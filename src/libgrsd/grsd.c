#include <sys/errno.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "grsd.h"
#include "itypes.h"
#include "log.h"
#include "session.h"

#define MAX(x, y) (x > y) ? x : y

grsd_t grsd_init() {
  struct _grsd* handle;

  if ((handle = malloc(sizeof(struct _grsd))) == NULL) {
    return NULL;
  }

  if (pipe(handle->listen_pipe) != 0) {
    free(handle);
    return NULL;
  }

  if ((handle->bind = ssh_bind_new()) == NULL) {
    close(handle->listen_pipe[0]);
    close(handle->listen_pipe[1]);
    free(handle);
    return NULL;
  }

  handle->listen_port = 22;
  handle->hostkey = NULL;
  handle->session_list = slist_init();

  return handle;
}

int grsd_destroy(grsd_t handle) {
  if (handle == NULL) {
    return -1;
  }

  slist_destroy(handle->session_list);
  close(handle->listen_pipe[0]);
  close(handle->listen_pipe[1]);
  ssh_bind_free(handle->bind);
  free(handle->hostkey);
  free(handle);

  return 0;
}

int grsd_get_listen_port(grsd_t handle) {

  if (handle == NULL) {
    return -1;
  }

  return handle->listen_port;
}

int grsd_set_listen_port(grsd_t handle, unsigned int port) {
  int result;

  if (handle == NULL) {
    return -1;
  }

  if (port < 1 || port > 65536) {
    return -1;
  }

  result = ssh_bind_options_set(handle->bind, SSH_BIND_OPTIONS_BINDPORT, &port);

  if (result == SSH_OK) {
    handle->listen_port = port;
    return 0;
  } else {
    return -1;
  }
}

const char* grsd_get_hostkey(grsd_t handle) {
  if (handle == NULL) {
    return NULL;
  }

  return handle->hostkey;
}

int grsd_set_hostkey(grsd_t handle, const char* path) {
  int result;

  if (handle == NULL || path == NULL) {
    return -1;
  }

  result = ssh_bind_options_set(handle->bind, SSH_BIND_OPTIONS_RSAKEY, path);

  if (result == SSH_OK) {
    if (handle->hostkey != NULL) {
      free(handle->hostkey);
    }

    if (asprintf(&handle->hostkey, path) == -1) {
      return -1;
    }

    return 0;
  } else {
    return -1;
  }
}

static int grsd_listen_handle_pipe(grsd_t handle) {
  ssize_t nread;
  char c;
  
  if ((nread = read(handle->listen_pipe[0], &c, 1)) != 1) {
    return -1; // Exit loop with -1
  }
  
  if (c == 'q') {
    return 0; // Exit loop with 0
  }
  
  return 1; // Ignored, stay in loop
}

static int grsd_listen_handle_ssh(grsd_t handle) {
  session_t session;
  
  if ((session = session_create(handle)) == NULL) {
    return -1; // Exit loop with -1
  }
  
  if (session_accept(session) != 0) {
    session_destroy(session);
    return 1; // Failed to accept the connection but stay in loop for further
              // login-attempts
  }
  
  
  slist_prepend(handle->session_list, session);
  
  return 1; // Stay in loop
}

static session_t find_selected_session(grsd_t handle, fd_set* rfds) {
  slist_it_t it = slist_iterator(handle->session_list);
  session_t session;
  
  while ((session = slist_iterator_next(it))) {
    if (FD_ISSET(ssh_get_fd(session->session), rfds)) {
      break;
    }
  }

  slist_iterator_destroy(it);
  return session;
}

int grsd_listen(grsd_t handle) {
  fd_set rfds;
  int exit_loop = 0;
  int exit_code = 0;

  log_debug("Enter grsd_listen");
  
  if (handle == NULL) {
    return -1;
  }

  if (ssh_bind_listen(handle->bind) < 0) {
    log_fatal("Error listening to socket: %s\n", ssh_get_error(handle->bind));
    return -1;
  }

  while (!exit_loop) {
    int max_fd, result;
    slist_it_t it;
    session_t session;

    log_debug("Another listen-loop");
    
    FD_ZERO(&rfds);
    FD_SET(handle->listen_pipe[0], &rfds);
    FD_SET(ssh_bind_get_fd(handle->bind), &rfds);
    max_fd = MAX(handle->listen_pipe[0], ssh_bind_get_fd(handle->bind));

    log_debug("Selecting %i sessions", slist_get_size(handle->session_list));
    it = slist_iterator(handle->session_list);
    
    while ((session = slist_iterator_next(it))) {
      FD_SET(ssh_get_fd(session->session), &rfds);
      max_fd = MAX(max_fd, ssh_get_fd(session->session));
      log_debug("Selecting session %i", ssh_get_fd(session->session));
    }
    
    slist_iterator_destroy(it);
    
    result = select(max_fd + 1, &rfds, NULL, NULL, NULL);

    if (result == -1 && errno != EINTR) {
      log_fatal("select: %s", strerror(errno));
      exit_loop = 1;
      break;
    }

    if (FD_ISSET(handle->listen_pipe[0], &rfds)) {
      log_debug("listen_pipe selected in read-set");
      
      if ((result = grsd_listen_handle_pipe(handle)) <= 0) {
        log_debug("listen_pipe-handling ended in %i. Leaving loop...", result);
        exit_loop = 1;
        exit_code = result;
      }
    } else if (FD_ISSET(ssh_bind_get_fd(handle->bind), &rfds)) {
      log_debug("bind_fd selected in read-set");
      
      if ((result = grsd_listen_handle_ssh(handle)) <= 0) {
        log_debug("bind_fd-handling ended in %i. Leaving loop...", result);
        exit_loop = 1;
        exit_code = result;
      }
    } else {
      // A session is selected, find the session-instance for the given fd
      session_t session = find_selected_session(handle, &rfds);
      log_debug("Session found for fd %i, handle session",
                ssh_get_fd(session->session));

      slist_remove(handle->session_list, session);
      session_destroy(session);
      
      log_debug("Remaining sessions: %i\n",
                slist_get_size(handle->session_list));
    }
  }

  log_debug("Leaving grsd_listen with %i", exit_code);
  return exit_code;
}

int grsd_listen_exit(grsd_t handle) {
  ssize_t nwritten;

  if (handle == NULL) {
    return -1;
  }

  log_debug("Ask to leave grsd_listen");
  nwritten = write(handle->listen_pipe[1], "q", 1);

  if (nwritten != 1) {
    return -1;
  }

  return 0;
}

#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "grsd.h"
#include "log.h"
#include "types.h"

static void handle_pipe(evutil_socket_t fd, short what, void* arg) {
  grsd_t handle = (grsd_t)arg;
  int c;
  size_t nread;
  
  log_debug("Handling incoming data from pipe");
  
  if ((nread = read(fd, &c, 1)) != 1) {
    log_fatal("Failed to read from pipe: %s", strerror(errno));
    event_base_loopexit(handle->event_base, NULL);
    return;
  }

  if (c == 'q') {
    log_debug("Quit-request received");
    event_base_loopexit(handle->event_base, NULL);
    return;
  }
}

static void handle_session(evutil_socket_t fd, short what, void* arg) {
  session_t session = (session_t)arg;
  
  // Handle the ssh-data
  // If the return-code is less than 0, the the handler asks to remove the
  // related event from the event_base_loop and destroy the session
  if (session_handle(session) < 0) {
    event_del(session->session_ev);
    session_destroy(session);
  }
}

static void handle_sshbind(evutil_socket_t fd, short what, void* arg) {
  grsd_t handle = (grsd_t)arg;
  session_t session;
  
  log_debug("New incoming connection-attempt");
  
  // Create a new session for the requested connection
  if ((session = session_create(handle)) == NULL) {
    // Fatal error: leave the event_base_loop
    log_fatal("Failed to create a session");
    grsd_listen_exit(handle);
    return;
  }
  
  // Accept the incoming connection
  if (session_accept(session) != 0) {
    // Failed to accept connection, stay in event_base_loop for further
    // login-attempts
    log_err("Failed to accept incoming connection");
    session_destroy(session);
    return;
  }
  
  // Initial handle the ssh-data
  // If the return-code is less than 0, the the handler asks to remove the
  // related event from the event_base_loop and destroy the session
  if (session_handle(session) < 0) {
    event_del(session->session_ev);
    session_destroy(session);
  }
  
  // Register the session-fd at event_base_loop
  session->session_ev = event_new(handle->event_base,
                                  ssh_get_fd(session->session),
                                  EV_READ|EV_PERSIST,
                                  handle_session,
                                  session);
  event_add(session->session_ev, NULL);
}

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
  handle->event_base = event_base_new();
  handle->sshbind_ev = NULL;
  handle->pipe_ev = event_new(handle->event_base,
                              handle->listen_pipe[0],
                              EV_READ|EV_PERSIST,
                              handle_pipe,
                              handle);
  event_add(handle->pipe_ev, NULL);
  
  return handle;
}

int grsd_destroy(grsd_t handle) {
  if (handle == NULL) {
    return -1;
  }

  close(handle->listen_pipe[0]);
  close(handle->listen_pipe[1]);
  ssh_bind_free(handle->bind);
  free(handle->hostkey);
  event_base_free(handle->event_base);
  
  if (handle->sshbind_ev != NULL) {
    event_free(handle->sshbind_ev);
  }

  event_del(handle->pipe_ev);
  event_free(handle->pipe_ev);
  
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

int grsd_listen(grsd_t handle) {
  int exit_code = 0;

  log_debug("Enter grsd_listen");
  
  if (handle == NULL) {
    return -1;
  }

  if (ssh_bind_listen(handle->bind) < 0) {
    log_fatal("Error listening to socket: %s\n", ssh_get_error(handle->bind));
    return -1;
  }

  handle->sshbind_ev = event_new(handle->event_base,
                                 ssh_bind_get_fd(handle->bind),
                                 EV_READ|EV_PERSIST,
                                 handle_sshbind,
                                 handle);
  event_add(handle->sshbind_ev, NULL);

  event_base_loop(handle->event_base, 0);

  event_del(handle->sshbind_ev);

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

  log_debug("Leave request issued");
  return 0;
}

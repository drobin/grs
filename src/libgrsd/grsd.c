#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "grsd.h"
#include "log.h"
#include "types.h"

extern void grsd_handle_pipe(evutil_socket_t, short, void*);
extern void grsd_handle_sshbind(evutil_socket_t, short, void*);

grsd_t grsd_init() {
  struct _grsd* handle;

  if ((handle = malloc(sizeof(struct _grsd))) == NULL) {
    return NULL;
  }

  // The ctrl_pipe
  if (pipe(handle->ctrl_pipe) != 0) {
    free(handle);
    return NULL;
  }

  // Create the libssh-sshbind
  if ((handle->bind = ssh_bind_new()) == NULL) {
    close(handle->ctrl_pipe[0]);
    close(handle->ctrl_pipe[1]);
    free(handle);
    return NULL;
  }

  // Some default SSH-daemon-information
  handle->listen_port = 22;
  handle->hostkey = NULL;

  // The event_base from libevent
  handle->event_base = event_base_new();

  // sshbind-event
  // The event cannot created now because the connection is not open
  handle->sshbind_ev = NULL;

  // Create the ctrl_pipe-event
  // It's safe to create the event already now because the pipe was already
  // created
  handle->pipe_ev = event_new(handle->event_base,
                              handle->ctrl_pipe[0],
                              EV_READ|EV_PERSIST,
                              grsd_handle_pipe,
                              handle);
  event_add(handle->pipe_ev, NULL);

  return handle;
}

int grsd_destroy(grsd_t handle) {
  if (handle == NULL) {
    return -1;
  }

  close(handle->ctrl_pipe[0]);
  close(handle->ctrl_pipe[1]);

  ssh_bind_free(handle->bind);

  free(handle->hostkey);

  // Destroy the libevent-event_base
  event_base_free(handle->event_base);

  // Destroy sshbind-event if not created
  if (handle->sshbind_ev != NULL) {
    event_free(handle->sshbind_ev);
  }

  // Destroy ctrl_pipe-event
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
                                 grsd_handle_sshbind,
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
  nwritten = write(handle->ctrl_pipe[1], "q", 1);

  if (nwritten != 1) {
    return -1;
  }

  log_debug("Leave request issued");
  return 0;
}

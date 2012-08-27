#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "grsd.h"
#include "log.h"

struct _grsd {
  /**
   * Instance to the SSH-server-bind.
   */
  ssh_bind bind;

  /**
   * Port where the SSH-server is listening.
   */
  unsigned int listen_port;

  /**
   * Hostkey of the SSH-server.
   */
  char* hostkey;
};

int grsd_bind_accept(grsd_t handle, ssh_session session) {
  int result;

  if ((result = ssh_bind_accept(handle->bind, session)) != SSH_OK) {
    log_err("Error accepting connection: %s", ssh_get_error(handle->bind));
  }

  return result;
}

grsd_t grsd_init() {
  struct _grsd* handle;

  if ((handle = malloc(sizeof(struct _grsd))) == NULL) {
    return NULL;
  }

  // Create the libssh-sshbind
  if ((handle->bind = ssh_bind_new()) == NULL) {
    free(handle);
    return NULL;
  }

  // Some default SSH-daemon-information
  handle->listen_port = 22;
  handle->hostkey = NULL;

  return handle;
}

int grsd_destroy(grsd_t handle) {
  if (handle == NULL) {
    return -1;
  }

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

  log_debug("Leaving grsd_listen with %i", exit_code);
  return exit_code;
}

int grsd_listen_get_fd(grsd_t handle) {
  if (handle == NULL || handle->bind == NULL) {
    return -1;
  }

  return ssh_bind_get_fd(handle->bind);
}

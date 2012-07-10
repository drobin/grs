#include <sys/errno.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "grsd.h"

#define MAX(x, y) (x > y) ? x : y

struct _grsd {
  int listen_pipe[2];
  ssh_bind bind;
  unsigned int listen_port;
  char* hostkey;
};

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
  fd_set rfds;
  int exit_loop = 0;
  int exit_code = 0;

  if (handle == NULL) {
    return -1;
  }

  if (ssh_bind_listen(handle->bind) < 0) {
    printf("Error listening to socket: %s\n",ssh_get_error(handle->bind));
    return -1;
  }

  while (!exit_loop) {
    int max_fd, result;

    FD_ZERO(&rfds);
    FD_SET(handle->listen_pipe[0], &rfds);
    FD_SET(ssh_bind_get_fd(handle->bind), &rfds);

    max_fd = MAX(handle->listen_pipe[0], ssh_bind_get_fd(handle->bind));

    result = select(max_fd + 1, &rfds, NULL, NULL, NULL);

    if (result == -1 && errno != EINTR) {
      exit_loop = 1;
      perror("select");
      break;
    }

    if (FD_ISSET(handle->listen_pipe[0], &rfds)) {
      ssize_t nread;
      char c;

      if ((nread = read(handle->listen_pipe[0], &c, 1)) != 1) {
        exit_loop = 1;
        break;
      }

      if (c == 'q') {
        exit_loop = 1;
        break;
      }
    }
  }

  return exit_code;
}

int grsd_listen_exit(grsd_t handle) {
  ssize_t nwritten;

  if (handle == NULL) {
    return -1;
  }

  nwritten = write(handle->listen_pipe[1], "q", 1);

  if (nwritten != 1) {
    return -1;
  }

  return 0;
}

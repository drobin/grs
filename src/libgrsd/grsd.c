#include <sys/errno.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "grsd.h"

struct _grsd {
  int listen_pipe[2];
  ssh_bind bind;
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

  return handle;
}

int grsd_destroy(grsd_t handle) {
  if (handle == NULL) {
    return -1;
  }

  close(handle->listen_pipe[0]);
  close(handle->listen_pipe[1]);
  ssh_bind_free(handle->bind);
  free(handle);

  return 0;
}

int grsd_listen(grsd_t handle) {
  fd_set rfds;
  int exit_loop = 0;

  if (handle == NULL) {
    return -1;
  }

  while (!exit_loop) {
    int max_fd = handle->listen_pipe[0];
    int result;

    FD_ZERO(&rfds);
    FD_SET(handle->listen_pipe[0], &rfds);

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

  return 0;
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

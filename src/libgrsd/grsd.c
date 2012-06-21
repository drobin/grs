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
  if (handle == NULL) {
    return -1;
  }

  return 0;
}

int grsd_listen_exit(grsd_t handle) {
  if (handle == NULL) {
    return -1;
  }

  return 0;
}

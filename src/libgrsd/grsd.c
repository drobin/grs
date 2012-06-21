#include <stdlib.h>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "grsd.h"

struct _grsd {
  ssh_bind bind;
};

grsd_t grsd_init() {
  struct _grsd* handle;

  if ((handle = malloc(sizeof(struct _grsd))) == NULL) {
    return NULL;
  }

  if ((handle->bind = ssh_bind_new()) == NULL) {
    free(handle);
    return NULL;
  }

  return handle;
}

int grsd_destroy(grsd_t handle) {
  if (handle == NULL) {
    return -1;
  }

  ssh_bind_free(handle->bind);
  free(handle);

  return 0;
}

#include <stdlib.h>
#include <string.h>

#include "grs.h"

struct _grsd {
};

grsd_t grsd_init() {
  struct _grsd* handle;

  if ((handle = malloc(sizeof(struct _grsd))) == NULL) {
    return NULL;
  }

  memset(handle, 0, sizeof(struct _grsd));

  return handle;
}

int grsd_destroy(grsd_t handle) {
  if (handle == NULL) {
    return -1;
  }

  free(handle);

  return 0;
}

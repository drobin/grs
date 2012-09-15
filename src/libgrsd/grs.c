#include <stdlib.h>
#include <string.h>

#include "grs.h"

struct _grs {
};

grs_t grsd_init() {
  struct _grs* handle;

  if ((handle = malloc(sizeof(struct _grs))) == NULL) {
    return NULL;
  }

  memset(handle, 0, sizeof(struct _grs));

  return handle;
}

int grsd_destroy(grs_t handle) {
  if (handle == NULL) {
    return -1;
  }

  free(handle);

  return 0;
}

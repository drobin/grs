#include <stdlib.h>
#include <string.h>

#include "grs.h"

struct _grs {
  process_env_t process_env;
};

grs_t grs_init() {
  struct _grs* handle;

  if ((handle = malloc(sizeof(struct _grs))) == NULL) {
    return NULL;
  }

  memset(handle, 0, sizeof(struct _grs));

  if ((handle->process_env = process_env_create()) == NULL) {
    free(handle);
  }

  return handle;
}

int grs_destroy(grs_t handle) {
  if (handle == NULL) {
    return -1;
  }

  process_env_destroy(handle->process_env);
  free(handle);

  return 0;
}

process_env_t grs_get_process_env(grs_t handle) {
  if (handle == NULL) {
    return NULL;
  }

  return handle->process_env;
}

/*******************************************************************************
 *
 * This file is part of grs.
 *
 * grs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * grs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with grs.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "grs.h"

struct _grs {
  acl_t acl;
  process_env_t process_env;
};

grs_t grs_init() {
  struct _grs* handle;

  if ((handle = malloc(sizeof(struct _grs))) == NULL) {
    return NULL;
  }

  memset(handle, 0, sizeof(struct _grs));

  if ((handle->acl = acl_init()) == NULL) {
    free(handle);
  }

  if ((handle->process_env = process_env_create()) == NULL) {
    acl_destroy(handle->acl);
    free(handle);
  }

  return handle;
}

int grs_destroy(grs_t handle) {
  if (handle == NULL) {
    return -1;
  }

  acl_destroy(handle->acl);
  process_env_destroy(handle->process_env);
  free(handle);

  return 0;
}

acl_t grs_get_acl(grs_t handle) {
  if (handle == NULL) {
    return NULL;
  }

  return handle->acl;
}

process_env_t grs_get_process_env(grs_t handle) {
  if (handle == NULL) {
    return NULL;
  }

  return handle->process_env;
}

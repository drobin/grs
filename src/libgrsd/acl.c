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

#include "acl.h"

struct _acl {
};

acl_t acl_init() {
  struct _acl* acl;

  if ((acl = malloc(sizeof(struct _acl))) == NULL) {
    return NULL;
  }

  return acl;
}

int acl_destroy(acl_t acl) {
  if (acl == NULL) {
    return -1;
  }

  free(acl);

  return 0;
}

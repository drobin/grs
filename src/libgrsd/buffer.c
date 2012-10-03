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

#include "buffer.h"

struct _buffer {
};

buffer_t buffer_create() {
  struct _buffer* buffer;

  if ((buffer = malloc(sizeof(struct _buffer))) == NULL) {
    return NULL;
  }

  memset(buffer, 0, sizeof(struct _buffer));

  return buffer;
}

int buffer_destroy(buffer_t buffer) {
  if (buffer == NULL) {
    return -1;
  }

  free(buffer);

  return 0;
}

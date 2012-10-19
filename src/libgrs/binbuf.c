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

#include "binbuf.h"

struct _binbuf {
  size_t size;
};

binbuf_t binbuf_create(size_t size) {
  struct _binbuf* buf;

  if ((buf = malloc(sizeof(struct _binbuf))) == NULL) {
    return NULL;
  }

  memset(buf, 0, sizeof(struct _binbuf));
  buf->size = size;

  return buf;
}

int binbuf_destroy(binbuf_t buf) {
  if (buf == NULL) {
    return -1;
  }

  free(buf);

  return 0;
}

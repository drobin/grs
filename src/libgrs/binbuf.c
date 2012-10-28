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
  size_t nelems;
  size_t capacity;
  void* data;
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

  free(buf->data);
  free(buf);

  return 0;
}

size_t binbuf_get_size_of(binbuf_t buf) {
  if (buf == NULL) {
    return -1;
  }

  return buf->size;
}

size_t binbuf_get_size(binbuf_t buf) {
  if (buf == NULL) {
    return -1;
  }

  return buf->nelems;
}

size_t binbuf_get_capacity(binbuf_t buf) {
  if (buf == NULL) {
    return -1;
  }

  return buf->capacity;
}

const void* binbuf_get(binbuf_t buf, size_t idx) {
  if (buf == NULL) {
    return NULL;
  }

  if (idx >= buf->nelems) {
    return NULL;
  }

  return buf->data + buf->size * idx;
}

int binbuf_find(binbuf_t buf, const void* cmp, size_t n) {
  int idx;
  size_t nbytes;

  if (buf == NULL || cmp == NULL) {
    return -1;
  }

  nbytes = (n > buf->size) ? buf->size : n;

  for (idx = 0; idx < buf->nelems; idx++) {
    if (memcmp(buf->data + buf->size * idx, cmp, nbytes) == 0) {
      return idx;
    }
  }

  return -1;
}

void* binbuf_add(binbuf_t buf) {
  if (buf == NULL) {
    return NULL;
  }

  // Increase capacity (if necessary)
  while (buf->capacity < buf->nelems + 1) {
    buf->data = realloc(buf->data, buf->size * (buf->capacity + 1));
    buf->capacity++;
  }

  buf->nelems++;
  return buf->data + buf->size * (buf->nelems - 1);
}

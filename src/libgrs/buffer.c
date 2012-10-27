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
  size_t capacity;
  size_t size;
  void* buf;
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

  free(buffer->buf);
  free(buffer);

  return 0;
}

size_t buffer_get_capacity(buffer_t buffer) {
  if (buffer == NULL) {
    return -1;
  }

  return buffer->capacity;
}

int buffer_ensure_capacity(buffer_t buffer, size_t capacity) {
  if (buffer == NULL) {
    return -1;
  }

  if (buffer->capacity < capacity) {
    buffer->buf = realloc(buffer->buf, capacity);
    buffer->capacity = capacity;
  }

  return 0;
}

size_t buffer_get_size(buffer_t buffer) {
  if (buffer == NULL) {
    return -1;
  }

  return buffer->size;
}

const void* buffer_get_data(buffer_t buffer) {
  if (buffer == NULL) {
    return NULL;
  }

  return buffer->buf;
}

int buffer_append(buffer_t buffer, const void* data, size_t nbytes) {
  if (buffer == NULL || data == NULL) {
    return -1;
  }

  buffer_ensure_capacity(buffer, buffer->size + nbytes);
  memcpy(buffer->buf + buffer->size, data, nbytes);
  buffer->size += nbytes;

  return 0;
}

int buffer_remove(buffer_t buffer, size_t nbytes) {
  if (buffer == NULL) {
    return -1;
  }

  if (buffer->size >= nbytes) {
    memcpy(buffer->buf, buffer->buf + nbytes, buffer->size - nbytes);
    buffer->size -= nbytes;
  } else {
    buffer->size = 0;
  }

  return 0;
}

int buffer_clear(buffer_t buffer) {
  if (buffer == NULL) {
    return -1;
  }

  buffer->size = 0;
  return 0;
}

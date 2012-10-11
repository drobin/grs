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
#include <stdio.h>
#include <string.h>

#include "protocol.h"

static int hex2int(char hex[4]) {
  int n, i;

  for (n = 0, i = 0; i < 4; i++) {
    int t;

    if (hex[i] >= 'A' && hex[i] <= 'F') {
      t = hex[i] - 'A' + 10;
    } else if (hex[i] >= 'a' && hex[i] <= 'f') {
      t = hex[i] - 'a' + 10;
    } else if (hex[i] >= '0' && hex[i] <= '9') {
      t = hex[i] - '0';
    } else {
      return n;
    }

    n = 16 * n + t;
  }

  return n;
}

struct pkt_line* pkt_line_create(int len, char* payload) {
  struct pkt_line* line;

  if (len == 0) { // flush_pkt
    if ((line = malloc(sizeof(struct pkt_line))) == NULL) {
      return NULL;
    }

    line->len = 0;
  } else {
    if (payload == NULL) {
      return NULL;
    }

    if ((line = malloc(sizeof(struct pkt_line) + len)) == NULL) {
      return NULL;
    }

    line->len = len;
    memcpy(line->payload, payload, len);
  }

  return line;
}

int pkt_line_destroy(struct pkt_line* line) {
  if (line == NULL) {
    return -1;
  }

  free(line);

  return 0;
}

struct pkt_line* pkt_line_read(buffer_t buf) {
  int size;

  if (buf == NULL) {
    return NULL;
  }

  // 4 bytes length-indicator, empty pkt-line "0004" is not allowed
  if (buffer_get_size(buf) < 4) {
    return NULL;
  }

  // Length of pkt-line
  size = hex2int(buffer_get_data(buf));

  // Empty pkt-line "0004" is not allowed
  if (size == 4) {
    return NULL;
  }

  if (size == 0) { // flush_pkt
    struct pkt_line* line = pkt_line_create(0, NULL);
    buffer_remove(buf, 4);

    return line;
  } else if (buffer_get_size(buf) < size) {
    // You don't have the complete pkt-line
    return NULL;
  } else {
    struct pkt_line* line = pkt_line_create(size - 4, buffer_get_data(buf) + 4);
    buffer_remove(buf, size);

    return line;
  }
}

int pkt_line_write(struct pkt_line* line, buffer_t buf) {
  if (line == NULL || buf == NULL) {
    return -1;
  }

  if (line->len == 0) { // flush-pkt
    return buffer_append(buf, "0000", 4);
  } else {
    char hex[5];

    snprintf(hex, 5, "%04x", line->len + 4);

    if (buffer_append(buf, hex, 4) == 0 &&
        buffer_append(buf, line->payload, line->len) == 0) {
      return 0;
    } else {
      return -1;
    }
  }
}

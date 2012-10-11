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

struct pkt_line* pkt_line_read(buffer_t buf) {
  struct pkt_line* line;
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
    if ((line = malloc(sizeof(struct pkt_line))) == NULL) {
      return NULL;
    }

    line->len = 0;
    buffer_remove(buf, 4);

    return line;
  }

  // Test if you have the complete pkt-line
  if (buffer_get_size(buf) < size) {
    return NULL;
  }

  // Allocate: size - 4 is payload-length
  if ((line = malloc(sizeof(struct pkt_line) + size - 4)) == NULL) {
    return NULL;
  }

  line->len = size - 4;
  memcpy(line->payload, buffer_get_data(buf) + 4, line->len);
  buffer_remove(buf, size);

  return line;
}

int pkt_line_destroy(struct pkt_line* line) {
  if (line == NULL) {
    return -1;
  }

  free(line);

  return 0;
}

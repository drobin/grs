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

#include "pkt_line.h"

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

int pkt_line_read(buffer_t src, buffer_t dest) {
  int size;

  if (src == NULL || dest == NULL) {
    return -1;
  }

  // 4 bytes length-indicator, empty pkt-line "0004" is not allowed
  if (buffer_get_size(src) < 4) {
    return -1;
  }

  // Length of pkt-line
  size = hex2int(buffer_get_data(src));

  // Empty pkt-line "0004" is not allowed
  if (size == 4) {
    return -1;
  }

  if (size == 0) { // flush_pkt
    buffer_remove(src, 4);
    return 0;
  } else if (buffer_get_size(src) < size) {
    // You don't have the complete pkt-line
    return -1;
  } else {
    buffer_append(dest, buffer_get_data(src) + 4, size - 4);
    buffer_remove(src, size);
    return 0;
  }
}

int pkt_line_write(buffer_t src, buffer_t dest) {
  if (src == NULL || dest == NULL) {
    return -1;
  }

  if (buffer_get_size(src) == 0) { // flush-pkt
    return buffer_append(dest, "0000", 4);
  } else {
    char hex[5];

    snprintf(hex, 5, "%04x", buffer_get_size(src) + 4);

    if (buffer_append(dest, hex, 4) == 0 &&
        buffer_append(dest, buffer_get_data(src), buffer_get_size(src)) == 0) {
      return 0;
    } else {
      return -1;
    }
  }
}

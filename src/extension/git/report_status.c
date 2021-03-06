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

#include "pkt_line.h"
#include "protocol.h"

int report_status(const char* repository, buffer_t out) {
  buffer_t pkt_line;

  if (repository == NULL || out == NULL) {
    return -1;
  }

  pkt_line = buffer_create();

  buffer_append(pkt_line, "unpack ok\n", 10); // unpack-status
  pkt_line_write(pkt_line, out);

  buffer_destroy(pkt_line);

  return 0;
}

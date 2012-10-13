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

int reference_discovery(buffer_t out, buffer_t err) {
  struct pkt_line* flush_pkt;

  if (out == NULL || err == NULL) {
    return -1;
  }

  flush_pkt = pkt_line_create(0, NULL);

  // No references: Simply send back a flush-pkt
  pkt_line_write(flush_pkt, out);
  pkt_line_destroy(flush_pkt);

  return 0;
}

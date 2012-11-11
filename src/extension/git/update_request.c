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

#include <libgrs/log.h>

#include "pkt_line.h"
#include "protocol.h"

int update_request(const char* repository, buffer_t in) {
  buffer_t pkt_line;
  int result = 0;

  if (repository == NULL || in == NULL) {
    return -1;
  }

  if (buffer_get_size(in) == 0) {
    // Nothing to do
    return 1;
  }

  pkt_line = buffer_create();

  log_debug("Start of update-request");

  while (1) {
    if ((result = pkt_line_read(in, pkt_line)) != 0) {
      if (result < 0) {
        log_err("Failed to read a pkt-line");
      }

      break;
    }

    if (buffer_get_size(pkt_line) == 0) {
      // flush-pkt, this is the end of the the update-request
      log_debug("End of update-request");
      break;
    } else {
      log_err("Currently only flush-pkt is supported on update-request");
      result = -1;
      break;
    }
  }

  buffer_destroy(pkt_line);

  return result == 0 ? 3 : result;
}

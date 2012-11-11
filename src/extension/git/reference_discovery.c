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

#include <libgrs/log.h>

#include "pkt_line.h"
#include "protocol.h"

int reference_discovery(const char* repository, enum git_process process,
                        buffer_t out, buffer_t err,
                        reference_discovery_cb refs) {
  binbuf_t refs_buf;
  buffer_t pkt;

  if (repository == NULL || out == NULL || err == NULL || refs == NULL) {
    return -1;
  }

  log_debug("Start of reference-discovery");

  refs_buf = binbuf_create(sizeof(struct git_ref));

  // Fetch the references
  if (refs(repository, refs_buf) != 0) {
    return -1;
  }

  if ((pkt = buffer_create()) == NULL) {
    return -1;
  }

  if (binbuf_get_size(refs_buf) == 0) {
    // No references: Simply send back a flush-pkt
    pkt_line_write(pkt, out);
  } else {
    int i;

    for (i = 0; i < binbuf_get_size(refs_buf); i++) {
      const struct git_ref* ref = binbuf_get(refs_buf, i);
      log_debug("ref %s %s", ref->obj_id, ref->ref_name);

      buffer_clear(pkt);
      buffer_append(pkt, ref->obj_id, strlen(ref->obj_id));
      buffer_append(pkt, " ", 1);
      buffer_append(pkt, ref->ref_name, strlen(ref->ref_name));
      buffer_append(pkt, "\n", 1);

      pkt_line_write(pkt, out);
    }

    binbuf_destroy(refs_buf);

    // flush_pkt
    buffer_clear(pkt);
    pkt_line_write(pkt, out);
  }

  log_debug("End of reference-discovery");
  buffer_destroy(pkt);

  return 0;
}

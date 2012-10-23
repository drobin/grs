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

#include <string.h>
#include <stdlib.h>

#include <libgrs/log.h>

#include "pkt_line.h"
#include "protocol.h"

int reference_discovery(const char* repository,
                        buffer_t out, buffer_t err, rd_get_refs refs) {
  binbuf_t refs_buf;
  buffer_t pkt;

  if (repository == NULL || out == NULL || err == NULL || refs == NULL) {
    return -1;
  }

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
      struct git_ref* ref = binbuf_get(refs_buf, i);

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

  buffer_destroy(pkt);

  return 0;
}

int packfile_negotiation(buffer_t in, struct packfile_negotiation_data* data) {
  if (in == NULL || data == NULL) {
    return -1;
  }

  if (data->phase == packfile_negotiation_prepare) {
    log_debug("Prepare packfile negotiation");
    data->pkt_line = buffer_create();
    data->want_list = binbuf_create(41);
    data->shallow_list = binbuf_create(41);
    data->depth = -1;

    // switch to next state
    data->phase++;
  }

  if (data->phase == packfile_negotiation_upload_request) {
    if (buffer_get_size(in) == 0) {
      // Nothing to do
      return 1;
    }

    if (binbuf_get_size(data->want_list) == 0) {
      log_debug("Start of upload-request");
    }

    while (1) {
      buffer_clear(data->pkt_line);
      if (pkt_line_read(in, data->pkt_line) != 0) {
        log_err("Failed to read a pkt-line");
        break;
      }

      if (buffer_get_size(data->pkt_line) == 0) {
        // flush-pkt, this is the end of the want-list, switch to next state
        log_debug("End of upload-request");
        data->phase++;
        break;
      }

      if (buffer_get_size(data->pkt_line) >= 5 &&
          strncmp(buffer_get_data(data->pkt_line), "want ", 5) == 0) {
        char* obj_id = binbuf_add(data->want_list);
        strlcpy(obj_id, buffer_get_data(data->pkt_line) + 5,
                binbuf_get_size_of(data->want_list));
        log_debug("want %s", obj_id);
      } else if (buffer_get_size(data->pkt_line) >= 8 &&
                 strncmp(buffer_get_data(data->pkt_line), "shallow ", 8) == 0) {
        char* obj_id = binbuf_add(data->shallow_list);
        strlcpy(obj_id, buffer_get_data(data->pkt_line) + 8,
                binbuf_get_size_of(data->shallow_list));
        log_debug("shallow %s", obj_id);
      } else if (buffer_get_size(data->pkt_line) >= 7 &&
                 strncmp(buffer_get_data(data->pkt_line), "deepen ", 7) == 0) {
        char num[buffer_get_size(data->pkt_line) - 7 + 1];
        strlcpy(num, buffer_get_data(data->pkt_line) + 7,
                buffer_get_size(data->pkt_line) - 7 + 1);
        log_debug("depth %s", num);
        data->depth = atoi(num);
      } else {
        log_err("Unsupported upload-request");
        data->phase = packfile_negotiation_error;
        break;
      }
    }
  }

  if (data->phase == packfile_negotiation_finished ||
      data->phase == packfile_negotiation_error) {
    log_debug("Cleanup packfile negotiation");
    buffer_destroy(data->pkt_line);
    binbuf_destroy(data->want_list);
    binbuf_destroy(data->shallow_list);
  }

  if (data->phase == packfile_negotiation_finished) {
    return 0;
  } else if (data->phase == packfile_negotiation_error) {
    return -1;
  } else {
    return 1;
  }
}

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

static void prepare(struct packfile_negotiation_data* data) {
  log_debug("Prepare packfile negotiation");
  data->pkt_line = buffer_create();
  data->want_list = binbuf_create(41);
  data->shallow_list = binbuf_create(41);
  data->have_list = binbuf_create(41);
  data->depth = -1;

  // switch to next state
  data->phase++;
}

static int upload_request(buffer_t in, struct packfile_negotiation_data* data) {
  if (buffer_get_size(in) == 0) {
    // Nothing to do
    return 1;
  }

  if (binbuf_get_size(data->want_list) == 0) {
    log_debug("Start of upload-request");
  }

  while (1) {
    int result;

    buffer_clear(data->pkt_line);
    if ((result = pkt_line_read(in, data->pkt_line)) != 0) {
      if (result < 0) {
        log_err("Failed to read a pkt-line");
        data->phase = packfile_negotiation_error;
      }

      break;
    }

    if (buffer_get_size(data->pkt_line) == 0) {
      // flush-pkt, this is the end of the want-list, switch to next state
      log_debug("End of upload-request");

      if (binbuf_get_size(data->want_list) > 0) {
        data->phase++;
      } else {
        // no wants -> no upload-request: switch to end
        data->phase = packfile_negotiation_quit;
      }

      break;
    }

    if (buffer_get_size(data->pkt_line) >= 5 &&
        strncmp(buffer_get_data(data->pkt_line), "want ", 5) == 0) {
      const char* want = buffer_get_data(data->pkt_line) + 5;
      if (binbuf_find(data->want_list, want, 40) == -1) {
        char* obj_id = binbuf_add(data->want_list);
        strlcpy(obj_id, want, binbuf_get_size_of(data->want_list));
        log_debug("want %s", obj_id);
      }
    } else if (buffer_get_size(data->pkt_line) >= 8 &&
               strncmp(buffer_get_data(data->pkt_line), "shallow ", 8) == 0) {
      const char* shallow = buffer_get_data(data->pkt_line) + 8;
      if (binbuf_find(data->shallow_list, shallow, 40) == -1) {
        char* obj_id = binbuf_add(data->shallow_list);
        strlcpy(obj_id, shallow, binbuf_get_size_of(data->shallow_list));
        log_debug("shallow %s", obj_id);
      }
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

  return 0;
}

static void upload_haves(buffer_t in, buffer_t out,
                         struct packfile_negotiation_data* data) {

  if (binbuf_get_size(data->have_list) == 0) {
    log_debug("Start of upload-haves");
  }

  while (1) {
    int result;

    buffer_clear(data->pkt_line);
    if ((result = pkt_line_read(in, data->pkt_line)) != 0) {
      if (result < 0) {
        log_err("Failed to read a pkt-line");
        data->phase = packfile_negotiation_error;
      }

      break;
    }

    if (buffer_get_size(data->pkt_line) >= 4 &&
        strncmp(buffer_get_data(data->pkt_line), "done", 4) == 0) {
      // end of upload-haves, switch to next phase
      log_debug("End of upload-haves");
      data->phase++;

      buffer_clear(data->pkt_line);
      buffer_append(data->pkt_line, "NAK\n", 4);
      pkt_line_write(data->pkt_line, out);

      break;
    } else {
      log_err("Unsupported upload-haves");
      data->phase = packfile_negotiation_error;
      break;
    }
  }
}

static void cleanup(struct packfile_negotiation_data* data) {
  log_debug("Cleanup packfile negotiation");
  buffer_destroy(data->pkt_line);
  binbuf_destroy(data->want_list);
  binbuf_destroy(data->shallow_list);
  binbuf_destroy(data->have_list);
}

int packfile_negotiation(const char* repository,buffer_t in, buffer_t out,
                         binbuf_t commits, commit_log_cb log_cb,
                         struct packfile_negotiation_data* data) {
  if (repository == NULL || in == NULL || out == NULL || commits == NULL ||
      log_cb == NULL || data == NULL) {
    return -1;
  }

  if (data->phase == packfile_negotiation_prepare) {
    prepare(data);
  }

  if (data->phase == packfile_negotiation_upload_request) {
    if (upload_request(in, data) > 0) {
      return 1;
    }
  }

  if (data->phase == packfile_negotiation_upload_haves) {
    upload_haves(in, out, data);
  }

  if (data->phase == packfile_negotiation_finished ||
      data->phase == packfile_negotiation_quit ||
      data->phase == packfile_negotiation_error) {

    int idx;

    // FIXME This is a very early implementation, simply copy all wants into the
    //       commits-array
    for (idx = 0; idx < binbuf_get_size(data->want_list); idx++) {
      char* hex = binbuf_add(commits);
      strlcpy(hex, binbuf_get(data->want_list, idx),
              binbuf_get_size_of(commits));
    }

    cleanup(data);
  }

  if (data->phase == packfile_negotiation_finished) {
    return 0;
  } else if (data->phase == packfile_negotiation_quit) {
    return 2;
  } else if (data->phase == packfile_negotiation_error) {
    return -1;
  } else {
    return 1;
  }
}

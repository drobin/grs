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

#include "capabilities.h"
#include "pkt_line.h"
#include "protocol.h"

static int capabilities_supported(int capabilities) {
  int result = capabilities;

  if (result & thin_pack) {
    result ^= thin_pack;
  }

  if (result & include_tag) {
    result ^= include_tag;
  }

  // TODO thin_pack and include_tag are marked as implemented but the
  //      implementation is still missing!

  return result == 0;
}

static int have_pkt_line(buffer_t pkt_line, const char* word) {
  const int len = strlen(word);
  const char* data = buffer_get_data(pkt_line);

  return buffer_get_size(pkt_line) >= len + 1 &&
         strncmp(data, word, len) == 0 && data[len] == ' ';
}

static void prepare(struct packfile_negotiation_data* data) {
  log_debug("Prepare packfile negotiation");
  data->pkt_line = buffer_create();
  data->want_list = binbuf_create(41);
  data->shallow_list = binbuf_create(41);
  data->have_list = binbuf_create(41);
  data->depth = 0;

  // switch to next state
  data->phase++;
}

static int upload_request(buffer_t in,
                          struct packfile_negotiation_result* pn_results,
                          struct packfile_negotiation_data* data) {
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

      if (!capabilities_supported(pn_results->capabilities)) {
        log_err("Unsupported capabilities received from client: 0x%02x",
          pn_results->capabilities);
        data->phase = packfile_negotiation_error;
      } else if (binbuf_get_size(data->shallow_list) > 0) {
        // FIXME This is a limitation and should be supported somehow/any time
        log_err("Currently no shallow-lines are supported");
        data->phase = packfile_negotiation_error;
      } else if (data->depth > 0) {
        // FIXME This is a limitation and should be supported somehow/any time
        log_err("Currently the deepen-line is not supported");
        data->phase = packfile_negotiation_error;
      } else if (binbuf_get_size(data->want_list) == 1) {
        data->phase++;
      } else if (binbuf_get_size(data->want_list) > 1) {
        // FIXME This is a limitation and should be supported somehow/any time
        log_err("Currently only one want-line is supported");
        data->phase = packfile_negotiation_error;
      } else {
        // no wants -> no upload-request: switch to end
        data->phase = packfile_negotiation_quit;
      }

      break;
    }

    if (have_pkt_line(data->pkt_line, "want")) {
      const char* want = buffer_get_data(data->pkt_line) + 5;
      if (binbuf_find(data->want_list, want, 40) == -1) {
        char* obj_id = binbuf_add(data->want_list);
        strlcpy(obj_id, want, binbuf_get_size_of(data->want_list));
        log_debug("want %s", obj_id);

        if (binbuf_get_size(data->want_list) == 1) {
          // first-want also contains capability-list
          const char* str = buffer_get_data(data->pkt_line) + 46;
          const size_t len = buffer_get_size(data->pkt_line) - 46;
          pn_results->capabilities = capabilities_parse(str, len);

          if (pn_results->capabilities == -1) {
            data->phase = packfile_negotiation_error;
            break;
          }
        }
      }
    } else if (have_pkt_line(data->pkt_line, "shallow")) {
      const char* shallow = buffer_get_data(data->pkt_line) + 8;
      if (binbuf_find(data->shallow_list, shallow, 40) == -1) {
        char* obj_id = binbuf_add(data->shallow_list);
        strlcpy(obj_id, shallow, binbuf_get_size_of(data->shallow_list));
        log_debug("shallow %s", obj_id);
      }
    } else if (have_pkt_line(data->pkt_line, "deepen")) {
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

static void upload_haves(const char* repository, buffer_t in, buffer_t out,
                         binbuf_t commits, commit_log_cb log_cb,
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

    if (have_pkt_line(data->pkt_line, "have")) {
      const char* have = buffer_get_data(data->pkt_line) + 5;
      char* obj_id = binbuf_add(data->have_list);
      strlcpy(obj_id, have, binbuf_get_size_of(data->have_list));
      log_debug("have %s", obj_id);
    } else if (buffer_get_size(data->pkt_line) >= 4 &&
               strncmp(buffer_get_data(data->pkt_line), "done", 4) == 0) {
      const char* hex;
      int common_base = -1;

      hex = binbuf_get(data->want_list, 0);
      if (log_cb(repository, hex, data->have_list, commits, &common_base) != 0) {
        data->phase = packfile_negotiation_error;
        break;
      }

      buffer_clear(data->pkt_line);

      if (common_base >= 0) {
        buffer_append(data->pkt_line, "ACK ", 4);
        buffer_append(data->pkt_line, binbuf_get(data->have_list, common_base),
                      binbuf_get_size_of(data->have_list) - 1);
        buffer_append(data->pkt_line, "\n", 1);
      } else {
        buffer_append(data->pkt_line, "NAK\n", 4);
      }

      pkt_line_write(data->pkt_line, out);

      // end of upload-haves, switch to next phase
      log_debug("End of upload-haves");
      data->phase++;

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
                         struct packfile_negotiation_result* pn_results,
                         commit_log_cb log_cb,
                         struct packfile_negotiation_data* data) {
  if (repository == NULL || in == NULL || out == NULL || pn_results == NULL ||
      log_cb == NULL || data == NULL) {
    return -1;
  }

  if (data->phase == packfile_negotiation_prepare) {
    prepare(data);
  }

  if (data->phase == packfile_negotiation_upload_request) {
    if (upload_request(in, pn_results, data) > 0) {
      return 1;
    }
  }

  if (data->phase == packfile_negotiation_upload_haves) {
    upload_haves(repository, in, out, pn_results->commits, log_cb, data);
  }

  if (data->phase == packfile_negotiation_finished ||
      data->phase == packfile_negotiation_quit ||
      data->phase == packfile_negotiation_error) {
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

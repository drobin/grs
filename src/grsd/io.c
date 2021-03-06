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

#include <sys/errno.h>
#include <errno.h>
#include <string.h>

#include <libgrs/log.h>
#include <libssh/server.h>

#include "io.h"

int buf2channel(struct session_list* list, struct session_entry* entry,
                int is_err) {
  const char* prefix = is_err ? "[Berr2C]" : "[Bout2C]";
  buffer_t buffer;

  if (entry->channel == NULL) {
    log_err("You don't have a destination-channel");
    return -1;
  }

  if (is_err) {
    buffer = session_get_err_buffer(entry->grs_session);
  } else {
    buffer = session_get_out_buffer(entry->grs_session);
  }

  log_debug("%s %i bytes available in buffer", prefix, buffer_get_size(buffer));

  while (buffer_get_size(buffer) > 0) {
    int nbytes;

    if (is_err) {
      nbytes = ssh_channel_write_stderr(entry->channel, buffer_get_data(buffer),
                                        buffer_get_size(buffer));
    } else {
      nbytes = ssh_channel_write(entry->channel, buffer_get_data(buffer),
                                 buffer_get_size(buffer));
    }

    if (nbytes == SSH_ERROR) {
      log_err("Failed to write into channel: %s",
              ssh_get_error(entry->channel));
      return -1;
    }

    log_debug("%s %i bytes written into channel", prefix, nbytes);
    log_data("B2C", buffer_get_data(buffer), nbytes);
    buffer_remove(buffer, nbytes);
  }

  return 0;
}

int channel2buf(struct session_list* list, struct session_entry* entry) {
  buffer_t buffer;
  size_t nread;

  nread = ssh_channel_read(entry->channel, list->buffer, sizeof(list->buffer), 0);
  if (nread == SSH_ERROR) {
    log_err("Failed to read from channel: %s", ssh_get_error(entry->channel));
    return -1;
  } else {
    log_debug("%i bytes read from channel", nread);
  }

  log_data("C2B", list->buffer, nread);

  buffer = session_get_in_buffer(entry->grs_session);
  if (buffer_append(buffer, list->buffer, nread) == 0) {
    log_debug("%i bytes written into in_buffer", nread);
    return 0;
  } else {
    log_err("Failed to write into in_buffer");
    return -1;
  }
}

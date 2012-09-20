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

#include <log.h>

#include "io.h"

int process2channel(struct session_list* list, struct session_entry* entry) {
  process_t process;
  size_t nread;
  size_t nwritten = 0;

  process = session_get_process(entry->grs_session);
  nread = read(process_get_fd_out(process), list->buffer, sizeof(list->buffer));

  if (nread == 0) {
    log_debug("EOF from fd_out");
    return -1;
  } else if (nread < 0) {
    log_err("Error occured while reading from process: %s", strerror(errno));
    return -1;
  }

  if (entry->channel == NULL) {
    log_err("You don't have a destination-channel");
    return -1;
  }

  log_debug("%i bytes read from process", nread);

  while (nwritten < nread) {
    int nbytes = ssh_channel_write(entry->channel,
                                   list->buffer + nwritten, nread - nwritten);

    if (nbytes == SSH_ERROR) {
      log_err("Failed to write into channel: %s",
              ssh_get_error(entry->channel));
      return -1;
    }

    nwritten += nbytes;
    log_debug("%i bytes written into channel", nbytes);
  }

  return 0;
}

int channel2process(struct session_list* list, struct session_entry* entry) {
  process_t process;
  size_t nread, nwritten;

  nread = ssh_channel_read(entry->channel, list->buffer, sizeof(list->buffer), 0);
  if (nread == SSH_ERROR) {
    log_err("Failed to read from channel: %s", ssh_get_error(entry->channel));
    return -1;
  } else {
    log_debug("%i bytes read from channel", nread);
  }

  if ((process = session_get_process(entry->grs_session)) == NULL) {
    log_err("You don't have a destination process");
    return -1;
  }

  nwritten = write(process_get_fd_in(process), list->buffer, nread);
  if (nwritten > 0) {
    log_debug("%i bytes written into process", nwritten);
    return 0;
  } else {
    log_err("Failed to write into process: %s", strerror(errno));
    return -1;
  }
}

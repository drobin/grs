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

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

enum LOG_LEVEL {
  LOG_INFO,
  LOG_WARN,
  LOG_ERR,
  LOG_FATAL,
  LOG_DEBUG
};

static void do_log(enum LOG_LEVEL level, const char* format, va_list ap) {
  char *fmt;

  switch (level) {
  case LOG_INFO:  asprintf(&fmt, "[INFO] %s\n", format); break;
  case LOG_WARN:  asprintf(&fmt, "[WARN] %s\n", format); break;
  case LOG_ERR:   asprintf(&fmt, "[ERR] %s\n", format); break;
  case LOG_FATAL: asprintf(&fmt, "[FATAL] %s\n", format); break;
  case LOG_DEBUG: asprintf(&fmt, "[DEBUG] %s\n", format);; break;
  }

  if (level == LOG_ERR || level == LOG_FATAL) {
    vfprintf(stderr, fmt, ap);
  } else {
    vfprintf(stdout, fmt, ap);
  }

  free(fmt);
}

void log_info(const char* format, ...) {
  va_list ap;

  va_start(ap, format);
  do_log(LOG_INFO, format, ap);
  va_end(ap);
}

void log_warn(const char* format, ...) {
  va_list ap;

  va_start(ap, format);
  do_log(LOG_WARN, format, ap);
  va_end(ap);
}

void log_err(const char* format, ...) {
  va_list ap;

  va_start(ap, format);
  do_log(LOG_ERR, format, ap);
  va_end(ap);
}

void log_fatal(const char* format, ...) {
  va_list ap;

  va_start(ap, format);
  do_log(LOG_FATAL, format, ap);
  va_end(ap);
}

void log_debug(const char* format, ...) {
  va_list ap;

  va_start(ap, format);
  do_log(LOG_DEBUG, format, ap);
  va_end(ap);
}

inline static char buf2char(char buf) {
  if (isprint(buf)) {
    return buf;
  } else {
    return '?';
  }
}

void log_data(const char* prefix, const char* buf, size_t nbytes) {
  const int block_size = 16;
  int i, block, nblocks;
  char hex_block[block_size * 10];
  char ascii_block[block_size * 10];

  nblocks = nbytes / block_size;
  if (nbytes % block_size > 0) {
    nblocks++;
  }

  for (block = 0; block < nblocks; block++) {
    hex_block[0] = '\0';
    ascii_block[0] = '\0';

    for (i = 0; i < block_size; i++) {
      int idx = block * block_size + i;

      if (idx >= nbytes) {
        break;
      }

      if (i == 0) {
        sprintf(hex_block, "%02X", (buf[idx] & 0xFF));
        sprintf(ascii_block, "%c", buf2char(buf[idx]));
      } else {
        sprintf(hex_block, "%s %02X", hex_block, (buf[idx] & 0xFF));
        sprintf(ascii_block, "%s%c", ascii_block, buf2char(buf[idx]));
      }
    }

    while (strlen(hex_block) < 3 * block_size) {
      strncat(hex_block, " ", 1);
    }

    fprintf(stdout, "[%s] %s%s\n", prefix, hex_block, ascii_block);
  }
}

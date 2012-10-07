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

#include "session.h"


struct _session {
  grs_t grs;
  buffer_t in_buf;
  buffer_t out_buf;
};

session_t session_create(grs_t grs) {
  struct _session* session;

  if (grs == NULL) {
    return NULL;
  }

  if ((session = malloc(sizeof(struct _session))) == NULL) {
    return NULL;
  }

  session->grs = grs;
  session->in_buf = buffer_create();
  session->out_buf = buffer_create();

  return session;
}

int session_destroy(session_t session) {
  if (session == NULL) {
    return -1;
  }

  buffer_destroy(session->in_buf);
  buffer_destroy(session->out_buf);
  free(session);

  return 0;
}

grs_t session_get_grs(session_t session) {
  if (session == NULL) {
    return NULL;
  }

  return session->grs;
}

int session_authenticate(session_t session,
                         const char* username, const char* password) {

  if (session == NULL || username == NULL || password == NULL) {
    return -1;
  }

  if (strcmp(username, password) == 0) {
    return 0;
  } else {
    return -1;
  }
}

buffer_t session_get_in_buffer(session_t session) {
  if (session == NULL) {
    return NULL;
  }

  return session->in_buf;
}

buffer_t session_get_out_buffer(session_t session) {
  if (session == NULL) {
    return NULL;
  }

  return session->out_buf;
}

int session_exec(session_t session) {
  if (session == NULL) {
    return -1;
  }

  return 0;
}

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
  enum session_state state;
  process_t process;
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
  session->state = NEED_AUTHENTICATION;
  session->process = NULL;

  return session;
}

int session_destroy(session_t session) {
  if (session == NULL) {
    return -1;
  }

  if (session->process != NULL) {
    process_destroy(session->process);
  }

  free(session);

  return 0;
}

grs_t session_get_grs(session_t session) {
  if (session == NULL) {
    return NULL;
  }

  return session->grs;
}

enum session_state session_get_state(session_t session) {
  if (session == NULL) {
    return -1;
  }

  return session->state;
}

int session_set_state(session_t session, enum session_state state) {
  if (session == NULL) {
    return -1;
  }

  session->state = state;

  return 0;
}

int session_authenticate(session_t session,
                         const char* username, const char* password) {

  if (session == NULL || username == NULL || password == NULL) {
    return -1;
  }

  if (session->state != NEED_AUTHENTICATION) {
    return -1;
  }

  if (strcmp(username, password) == 0) {
    session->state = NEED_PROCESS;
    return 0;
  } else {
    return -1;
  }
}

process_t session_get_process(session_t session) {
  if (session == NULL) {
    return NULL;
  }

  return session->process;
}

process_t session_create_process(session_t session, process_env_t env,
                                 const char* command) {
  if (session == NULL || env == NULL || command == NULL) {
    return NULL;
  }

  if (session->state != NEED_PROCESS) {
    return NULL;
  }

  session->process = process_prepare(env, command);
  session->state = NEED_EXEC;

  return session->process;
}

int session_exec(session_t session) {
  if (session == NULL) {
    return -1;
  }

  if (session->state != NEED_EXEC) {
    return -1;
  }

  process_exec(session->process);
  session->state = EXECUTING;

  return 0;
}

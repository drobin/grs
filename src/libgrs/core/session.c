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

#include <sys/param.h>
#include <stdlib.h>
#include <string.h>

#include "session.h"

struct _session {
  grs_t grs;
  char* raw_command;
  char* command[ARG_MAX];
  buffer_t in_buf;
  buffer_t out_buf;
  buffer_t err_buf;
  int exec_finished;
};

static void tokenize(struct _session* session) {
  char* token;
  int nargs = 0;

  for (; (token = strsep(&session->raw_command, " \t")) != NULL; nargs++) {
    session->command[nargs] = token;
  }

  session->command[nargs] = NULL;
}

session_t session_create(grs_t grs) {
  struct _session* session;

  if (grs == NULL) {
    return NULL;
  }

  if ((session = malloc(sizeof(struct _session))) == NULL) {
    return NULL;
  }

  session->grs = grs;
  session->raw_command = NULL;
  session->command[0] = NULL;
  session->in_buf = buffer_create();
  session->out_buf = buffer_create();
  session->err_buf = buffer_create();
  session->exec_finished = 0;

  return session;
}

int session_destroy(session_t session) {
  if (session == NULL) {
    return -1;
  }

  free(session->raw_command);
  buffer_destroy(session->in_buf);
  buffer_destroy(session->out_buf);
  buffer_destroy(session->err_buf);
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

const char** session_get_command(session_t session) {
  if (session == NULL) {
    return NULL;
  }

  return (const char**)session->command;
}

int session_set_command(session_t session, const char* command) {
  if (session == NULL || command == NULL) {
    return -1;
  }

  session->raw_command = strdup(command);
  tokenize(session);

  return 0;
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

buffer_t session_get_err_buffer(session_t session) {
  if (session == NULL) {
    return NULL;
  }

  return session->err_buf;
}

int session_can_exec(session_t session) {
  if (session == NULL) {
    return -1;
  }

  return session->command[0] != NULL;
}

int session_exec(session_t session) {
  command_hook hook;

  if (session == NULL) {
    return -1;
  }

  if ((hook = grs_get_command(session->grs, session->command[0])) != NULL) {
    int result = hook((const char**)session->command,
                      session->in_buf, session->out_buf, session->err_buf);
    session->exec_finished = (result != 1);

    return result;
  } else {
    session->exec_finished = 1;
    return -1;
  }
}

int session_is_finished(session_t session) {
  if (session == NULL) {
    return -1;
  }

  return session->exec_finished;
}

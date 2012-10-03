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

#ifndef SESSION_H
#define SESSION_H

#include "buffer.h"
#include "grs.h"
#include "process.h"

struct _session;
typedef struct _session* session_t;

enum session_state {
  NEED_AUTHENTICATION,
  NEED_PROCESS,
  NEED_EXEC,
  EXECUTING
};

session_t session_create(grs_t grs);
int session_destroy(session_t session);
grs_t session_get_grs(session_t session);

enum session_state session_get_state(session_t session);
int session_set_state(session_t session, enum session_state state);

int session_authenticate(session_t session,
                         const char* username, const char* password);
process_t session_get_process(session_t session);
process_t session_create_process(session_t session, process_env_t env,
                                 const char* command);

/**
 * Returns the buffer, which is consumed by the session-process.
 *
 * You put data into this buffer and the process consumes the data.
 *
 * @param session The requested session
 * @return The buffer you have to fill with data.
 */
buffer_t session_get_in_buffer(session_t session);

/**
 * Returns the buffer, which is filled by the session-process.
 *
 * The process fills the buffer with data, which needs to be send back to the
 * client.
 *
 * @param session The requested session
 * @return The buffer you have to send back to the client
 */
buffer_t session_get_out_buffer(session_t session);

int session_exec(session_t session);

#endif  /* SESSION_H */

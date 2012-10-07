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

struct _session;
typedef struct _session* session_t;

session_t session_create(grs_t grs);
int session_destroy(session_t session);
grs_t session_get_grs(session_t session);

int session_authenticate(session_t session,
                         const char* username, const char* password);

/**
 * Returns the command, which is assigned to the session.
 *
 * A NULL-terminated array is returned. Each array-element is a argument of the
 * command. If the array is empty, then no command was assigned.
 *
 * @param session The requested session
 * @return The tokenized command assigned to the session
 */
const char** session_get_command(session_t session);

/**
 * Assigns a command to the session.
 *
 * Later, this command is executed with session_exec().
 *
 * @param session The destination session
 * @param command The command to be assigned
 * @return On success <code>0</code> is returned.
 */
int session_set_command(session_t session, const char* command);

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

/**
 * Returns the buffer, which is filled by the session-process with
 * error-informations.
 *
 * The process filles the buffer with error-data, which needs to be send back
 * to the client.
 *
 * @param session The requested session
 * @return The error-buffer you have to send back to the client
 */
buffer_t session_get_err_buffer(session_t session);

/**
 * Tests whether you can exec the session via session_exec().
 *
 * You need at least a command (session_set_command()) assigned to the session.
 *
 * @param session The requested session
 * @return If you can exec the session, <code>true</code> is returned.
 */
int session_can_exec(session_t session);

/**
 * Executes the session.
 *
 * You can execute a session, when it is ready for execution. Check
 * session_can_exec().
 *
 * If the process needs more data to complete the exec-operation,
 * then <code>1<code> is returned.
 *
 * If the exec-operation completed its operation, then <code>0</code> is
 * returned. The session can be destroyed. <b>Note:</b> Even if the operation
 * is finished, there can be still data to be written back to the client. So
 * make sure to write back the output-buffer in any case!
 *
 * @param session The session to execute
 * @return If the process has finished execution, then <code>0</code> is
 *         returned, but if you need another run of session_exec()
 *         <code>1</code> is returned. If an error occured <code>-1</code> is
 *         returned.
 */
int session_exec(session_t session);

/**
 * Tests whether session_exec() has finished execution.
 *
 * If session_exec() was never executed, then this function returned
 * <code>false</code>. But whenever the exec-operation completed its operation,
 * then <code>true</code> is returned.
 *
 * @param session The requested session
 * @return If the session has completed its operation, then <code>true</code>
 *         is returned.
 */
int session_is_finished(session_t session);

#endif  /* SESSION_H */

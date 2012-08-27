#ifndef SESSION_H
#define SESSION_H

#include "grsd.h"

struct _session;

/**
 * Handle to the session-structure.
 */
typedef struct _session* session_t;

enum session_state {
  NOP,
  AUTH,
  CHANNEL_OPEN,
  REQUEST_CHANNEL
};

/**
 * Creates a new session-structure.
 *
 * @return A new session or <code>NULL</code> on error.
 */
session_t session_create();

/**
 * Destroys the session again.
 *
 * @param session The session to be destroyed
 */
int session_destroy(session_t session);

/**
 * Returns the current state of the session.
 *
 * @param session The requested session
 * @return The current state of the session
 * @todo This should not be a part of the public API.
 */
enum session_state session_get_state(session_t session);

/**
 * Updates the state of the session.
 *
 * @param session The requested session
 * @return On success <code>0</code> is returned.
 */
int session_set_state(session_t session, enum session_state state);

/**
 * Accepts a connection, which is assigned to the given session.
 *
 * @param session The destination session
 * @param handle The handle to the grs-daemon
 * @return On success <code>0</code> is returned, on error <code>-1</code>.
 */
int session_accept(session_t session, grsd_t handle);

/**
 * Returns the filedescriptor or the client-connection.
 *
 * @param session The destination session
 * @return The fd of the connection
 */
int session_get_fd(session_t session);

/**
 * Handles incoming data for the session.
 *
 * The return-value specifies what to do with the session:
 *
 * <ul>
 *  <li><code>-1</code>:
 *    A fatal error occured, the session needs to be destroyed
 *  </li>
 *  <li><code>0</code>:
 *    Data were processed but the session should stay open because further data
 *    are required.
 *  </li>
 *  <li><code>1</code>:
 *    Data where processed and no more data are required. You can close the
 *    session.
 *  </li>
 * </ul>
 */
int session_handle(session_t session);

int session_multiplex(session_t session, int read_fd, int write_fd);

#endif  /* SESSION_H */

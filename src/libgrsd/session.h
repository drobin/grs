#ifndef SESSION_H
#define SESSION_H

#include "types.h"

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

#endif  /* SESSION_H */
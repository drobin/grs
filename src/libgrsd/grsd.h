#ifndef GRSD_H
#define GRSD_H

struct _grsd;
struct _session;

/**
 * Handle to the grs-daemon-process.
 */
typedef struct _grsd* grsd_t;

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

/*
 * Creates a new grsd-handle.
 *
 * @return A new grsd-handle or <code>NULL</code> on error.
 */
grsd_t grsd_init();

/**
 * Destroys the grsd-handle again.
 *
 * @param handle The handle to be destroyed.
 * @return On success <code>0</code> is returned, on error <code>-1</code>.
 */
int grsd_destroy(grsd_t handle);

/**
 * Returns the port, where the ssh-daemon is listening on.
 *
 * @param handle The handle to the grsd-module
 * @return The listening-port of the ssh-daemon.
 */
int grsd_get_listen_port(grsd_t handle);

/**
 * Updates the listening-port of the ssh-daemon.
 *
 * @param handle The handle to the grsd-module
 * @param port The new port
 * @return On success 0 is returned. If the port is out of range, -1 is
 *         returned.
 */
int grsd_set_listen_port(grsd_t handle, unsigned int port);

/**
 * Returns the filename where the (RSA-) hostkey of the daemon is stored.
 *
 * @param handle The handle to the grsd-module
 * @return The path to the (RSA-) hostkey file. Of no file is assigned,
 *         <code>NULL</code> is returned.
 */
const char* grsd_get_hostkey(grsd_t handle);

/**
 * Assigns a filename, where the (RSA-) hostkey of the daemon is stored.
 *
 * @param handle The handle to the grsd-module
 * @param path The path to the hostkey-file
 * @return On success <code>0</code> is returned, on any error <code>-1</code>.
 */
int grsd_set_hostkey(grsd_t handle, const char* path);

/**
 * Main listening-loop.
 *
 * This method blocks the calling thread until grsd_listen_exit() is called
 * from another thread.
 *
 * @param handle The handle to the grsd-module
 * @return If the listening-loop was aborted gracefully, <code>0</code> is
 *         returned.
 */
int grsd_listen(grsd_t handle);

/**
 * Requests to leave the grsd_listen-loop.
 *
 * @param handle The handle to the grsd-module
 * @return If the request was issued, then <code>0</code> is returned. On error
 *         the return value is <code>-1</code>.
 */
int grsd_listen_exit(grsd_t handle);

/**
 * Creates a new session-structure.
 *
 * @param handle Handle to the parent grsd-daemon
 * @return A new session or <code>NULL</code> on error.
 */
session_t session_create(grsd_t handle);

/**
 * Destroys the session again.
 *
 * @param session The session to be destroyed
 */
int session_destroy(session_t session);

/**
 * Returns the parent grsd_t-instance of the session.
 *
 * @param session The requested session
 * @return The parent grsd_t-handle
 */
grsd_t session_get_grsd(session_t session);

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
 * @return On success <code>0</code> is returned, on error <code>-1</code>.
 */
int session_accept(session_t session);

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

#endif /* GRSD_H */

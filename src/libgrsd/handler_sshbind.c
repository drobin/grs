#include "grsd.h"
#include "handler.h"
#include "log.h"
#include "types.h"

void grsd_handle_sshbind(evutil_socket_t fd, short what, void* arg) {
  grsd_t handle = (grsd_t)arg;
  session_t session;

  log_debug("New incoming connection-attempt");

  // Create a new session for the requested connection
  if ((session = session_create(handle)) == NULL) {
    // Fatal error: leave the event_base_loop
    log_fatal("Failed to create a session");
    grsd_listen_exit(handle);
    return;
  }

  // Accept the incoming connection
  if (session_accept(session) != 0) {
    // Failed to accept connection, stay in event_base_loop for further
    // login-attempts
    log_err("Failed to accept incoming connection");
    session_destroy(session);
    return;
  }

  // Register the session-fd at event_base_loop
  session->session_ev = event_new(handle->event_base,
                                  ssh_get_fd(session->session),
                                  EV_READ|EV_PERSIST,
                                  grsd_handle_session,
                                  session);
  event_add(session->session_ev, NULL);

  // Initial handle the ssh-data
  event_active(session->session_ev, EV_READ, 1);
}

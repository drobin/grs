#include "grsd.h"
#include "types.h"

void grsd_handle_session(evutil_socket_t fd, short what, void* arg) {
  session_t session = (session_t)arg;
  
  // Handle the ssh-data
  // If the return-code is less than 0, the the handler asks to remove the
  // related event from the event_base_loop and destroy the session
  if (session_handle(session) < 0) {
    event_del(session->session_ev);
    session_destroy(session);
  }
}

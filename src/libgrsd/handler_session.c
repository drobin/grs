#include <string.h>

#include "grsd.h"
#include "log.h"
#include "process.h"
#include "types.h"

void grsd_handle_session(evutil_socket_t fd, short what, void* arg) {
  session_t session = (session_t)arg;

  if (session_handle(session) != 0) {
    session_destroy(session);
  }
}

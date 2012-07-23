#include <sys/errno.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "grsd.h"
#include "log.h"
#include "types.h"

void grsd_handle_pipe(evutil_socket_t fd, short what, void* arg) {
  grsd_t handle = (grsd_t)arg;
  int c;
  size_t nread;
  
  log_debug("Handling incoming data from pipe");
  
  if ((nread = read(fd, &c, 1)) != 1) {
    log_fatal("Failed to read from pipe: %s", strerror(errno));
    event_base_loopexit(handle->event_base, NULL);
    return;
  }
  
  if (c == 'q') {
    log_debug("Quit-request received");
    event_base_loopexit(handle->event_base, NULL);
    return;
  }
}

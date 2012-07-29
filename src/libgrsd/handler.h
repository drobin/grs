#ifndef HANDLER_H
#define HANDLER_H

#include <event.h>

void grsd_handle_pipe(evutil_socket_t fd, short what, void* arg);
void grsd_handle_sshbind(evutil_socket_t fd, short what, void* arg);
void grsd_handle_session(evutil_socket_t fd, short what, void* arg);

#endif  /* HANDLER_H */

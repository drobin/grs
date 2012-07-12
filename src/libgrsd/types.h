#ifndef TYPES_H
#define TYPES_H

#include <libssh/libssh.h>
#include <libssh/server.h>
#include <event.h>

enum SESSION_STATE {
  NOP,
  AUTH,
  CHANNEL_OPEN,
  REQUEST_CHANNEL
};

struct _grsd {
  int listen_pipe[2];
  ssh_bind bind;
  unsigned int listen_port;
  char* hostkey;
  struct event_base* event_base;
  struct event* sshbind_ev;
  struct event* pipe_ev;
};

struct _session {
  struct _grsd* handle;
  ssh_session session;
  ssh_channel channel;
  struct event* session_ev;
  enum SESSION_STATE state;
};

#endif  /* TYPES_H */
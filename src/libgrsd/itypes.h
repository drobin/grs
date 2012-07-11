#ifndef ITYPES_H
#define ITYPES_H

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "slist.h"

enum SESSION_STATE {
  NOP,
  AUTH,
  CHANNEL_OPEN
};

struct _grsd {
  int listen_pipe[2];
  ssh_bind bind;
  unsigned int listen_port;
  char* hostkey;
  slist_t session_list;
};

struct _session {
  struct _grsd* handle;
  ssh_session session;
  ssh_channel channel;
  enum SESSION_STATE state;
};

#endif  /* ITYPES_H */
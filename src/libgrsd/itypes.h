#ifndef ITYPES_H
#define ITYPES_H

#include <libssh/libssh.h>
#include <libssh/server.h>

struct _grsd {
  int listen_pipe[2];
  ssh_bind bind;
  unsigned int listen_port;
  char* hostkey;
};

struct _session {
  struct _grsd* handle;
  ssh_session session;
};

#endif  /* ITYPES_H */
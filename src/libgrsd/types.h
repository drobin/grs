#ifndef TYPES_H
#define TYPES_H

#include <libssh/libssh.h>
#include <libssh/server.h>

// TODO Don't forget to remove this include after a successful refactoring
#include "grsd.h"
#include "session.h"

struct _grsd {
  /**
   * Instance to the SSH-server-bind.
   */
  ssh_bind bind;

  /**
   * Port where the SSH-server is listening.
   */
  unsigned int listen_port;

  /**
   * Hostkey of the SSH-server.
   */
  char* hostkey;
};

struct _session {
  /**
   * The SSH-session.
   */
  ssh_session session;

  /**
   * The SSH-channel.
   */
  ssh_channel channel;

  /**
   * State of the session.
   */
  enum session_state state;
};

#endif  /* TYPES_H */

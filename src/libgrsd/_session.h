#ifndef _SESSION_H
#define _SESSION_H

#include <libssh/libssh.h>
#include <libssh/server.h>

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

#endif  /* _SESSION_H */

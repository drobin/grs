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
  /**
   * The control-pipe is used to send commands into the event_base_loop.
   */
  int ctrl_pipe[2];
  
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
  
  /**
   * The event_base of the event-loop.
   */
  struct event_base* event_base;
  
  /**
   * Event is used to receive information from the SSH-server-bind.
   */
  struct event* sshbind_ev;
  
  /**
   * Event is used to receive information from the ctrl_pipe.
   */
  struct event* pipe_ev;
};

struct _session {
  /**
   * Handle to the parent _grsd-structure.
   */
  struct _grsd* handle;
  
  /**
   * The SSH-session.
   */
  ssh_session session;
  
  /**
   * The SSH-channel.
   */
  ssh_channel channel;
  
  /**
   * Event is used to receive information from the session.
   */
  struct event* session_ev;
  
  /**
   * Event is used to read data from stdout of the forked process.
   */
  struct event* stdout_ev;
  
  /**
   * PID of the forked process.
   */
  pid_t pid;
  
  /**
   * State of the session.
   */
  enum SESSION_STATE state;
};

#endif  /* TYPES_H */
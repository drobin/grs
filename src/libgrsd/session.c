#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "log.h"
#include "process.h"
#include "session.h"

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


extern int grsd_bind_accept(grsd_t, ssh_session);

static int handle_auth(session_t session, ssh_message msg, int msg_type) {
  char* user;
  char* password;
  int msg_subtype;

  log_debug("Handle authentication for session");

  if (msg_type != SSH_REQUEST_AUTH) {
    log_debug("Ignoring message of type %i", msg_type);

    ssh_message_reply_default(msg);

    return 0;
  }

  if ((msg_subtype = ssh_message_subtype(msg)) != SSH_AUTH_METHOD_PASSWORD) {
    log_debug("Currently only password-authentication is supported.");
    log_debug("Rejecting auth-type %i", msg_subtype);

    ssh_message_auth_set_methods(msg, SSH_AUTH_METHOD_PASSWORD);
    ssh_message_reply_default(msg);

    return 0;
  }

  user = ssh_message_auth_user(msg);
  password = ssh_message_auth_password(msg);
  log_debug("Authentication requested with %s/%s", user, password);

  if (strcmp(user, password)) {
    log_debug("Authentication rejected");
    ssh_message_reply_default(msg);
    return 0;
  }

  log_debug("Authentication succeeded");
  ssh_message_auth_reply_success(msg, 0);
  session->state = CHANNEL_OPEN;

  return 0;
}

static int handle_channel_open(session_t session, ssh_message msg,
                               int msg_type) {

  log_debug("Handle open-request for a channel");

  if (msg_type != SSH_REQUEST_CHANNEL_OPEN) {
    log_debug("Ignoring message of type %i", msg_type);

    ssh_message_reply_default(msg);

    return 0;
  }

  session->channel = ssh_message_channel_request_open_reply_accept(msg);
  if (session->channel != NULL) {
    log_debug("Channel is open");
    session->state = REQUEST_CHANNEL;

    return 0;
  } else {
    log_err("Failed to open channel: %s", ssh_get_error(session->session));
    return -1;
  }
}

static int handle_request_channel(session_t session, ssh_message msg,
                                  int msg_type) {

  struct grs_process process;
  int msg_subtype;

  log_debug("Handle channel-request");

  if (msg_type != SSH_REQUEST_CHANNEL) {
    log_debug("Ignoring message of type %i", msg_type);

    ssh_message_reply_default(msg);

    return 0;
  }

  if ((msg_subtype = ssh_message_subtype(msg)) != SSH_CHANNEL_REQUEST_EXEC) {
    log_debug("Ignoring channel-type %i", msg_subtype);

    ssh_message_reply_default(msg);

    return 1;
  }

  ssh_message_channel_request_reply_success(msg);
  log_debug("Channel request accepted");

  grs_process_prepare(&process, ssh_message_channel_request_command(msg));
  grs_process_exec(&process, session->channel);

  session->state = NOP; // Finished

  return 1;
}

session_t session_create() {
  struct _session* session;

  if ((session = malloc(sizeof(struct _session))) == NULL) {
    return NULL;
  }

  if ((session->session = ssh_new()) == NULL) {
    free(session);
    return NULL;
  }

  session->channel = NULL;
  session->state = AUTH;

  return session;
}

int session_destroy(session_t session) {
  if (session == NULL) {
    return -1;
  }

  if (session->channel != NULL) {
    ssh_channel_free(session->channel);
  }

  ssh_free(session->session);
  free(session);

  return 0;
}

enum session_state session_get_state(session_t session) {
  if (session != NULL) {
    return session->state;
  } else {
    return -1;
  }
}

int session_set_state(session_t session, enum session_state state) {
  if (session != NULL) {
    session->state = state;
    return 0;
  } else {
    return -1;
  }
}

int session_accept(session_t session, grsd_t handle) {
  if (session == NULL || handle == NULL) {
    return -1;
  }

  if (grsd_bind_accept(handle, session->session) != SSH_OK) {
    return -1;
  }

  if (ssh_handle_key_exchange(session->session) != SSH_OK) {
    log_err("Error in key exchange: %s", ssh_get_error(session->session));
    return -1;
  }

  log_info("Connection established");
  // TODO Who is connected?

  return 0;
}

int session_get_fd(session_t session) {
  if (session == NULL || !ssh_is_connected(session->session)) {
    return -1;
  }

  return ssh_get_fd(session->session);
}

int session_handle(session_t session) {
  ssh_message msg;
  int msg_type;
  int result;

  if (session == NULL) {
    return -1; // -1: Fatal error, destroy session
  }

  log_debug("Handle incoming data from session");

  if ((msg = ssh_message_get(session->session)) == NULL) {
    log_err("Failed to read message from session: %s",
            ssh_get_error(session->session));
    return -1;
  }

  if ((msg_type = ssh_message_type(msg)) == -1) {
    log_debug("ssh_message_type of -1 received. Abort...");
    return -1; // -1: Fatal error, destroy session
  }

  // Depending on the session-state handle the ssh-data.
  switch (session->state) {
    case AUTH:
      result = handle_auth(session, msg, msg_type); break;
    case CHANNEL_OPEN:
      result = handle_channel_open(session, msg, msg_type); break;
    case REQUEST_CHANNEL:
      result = handle_request_channel(session, msg, msg_type); break;
    case NOP:
      result = -1; break;
  }

  ssh_message_free(msg);
  return result;
}

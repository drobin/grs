#include <string.h>

#include "grsd.h"
#include "log.h"
#include "process.h"
#include "types.h"

static int session_handle_auth(session_t session, ssh_message msg) {
  char* user;
  char* password;
  int msg_type, msg_subtype;

  log_debug("Handle authentication for session");

  if ((msg_type = ssh_message_type(msg)) != SSH_REQUEST_AUTH) {
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

static int session_handle_channel_open(session_t session, ssh_message msg) {
  log_debug("Handle open-request for a channel");

  if (ssh_message_type(msg) != SSH_REQUEST_CHANNEL_OPEN) {
    log_debug("Ignoring message of type %i", ssh_message_type(msg));

    ssh_message_reply_default(msg);

    return 0;
  }

  session->channel = ssh_message_channel_request_open_reply_accept(msg);
  log_debug("Channel is open");

  session->state = REQUEST_CHANNEL;

  return 0;
}

static int session_handle_request_channel(session_t session, ssh_message msg) {
  struct grs_process process;

  log_debug("Handle channel-request");

  if (ssh_message_type(msg) != SSH_REQUEST_CHANNEL) {
    log_debug("Ignoring message of type %i", ssh_message_type(msg));

    ssh_message_reply_default(msg);

    return 0;
  }

  if (ssh_message_subtype(msg) != SSH_CHANNEL_REQUEST_EXEC) {
    log_debug("Ignoring channel-type %i", ssh_message_subtype(msg));

    ssh_message_reply_default(msg);

    return -1;
  }

  ssh_message_channel_request_reply_success(msg);
  log_debug("Channel request accepted");

  grs_process_prepare(&process, ssh_message_channel_request_command(msg));
  grs_process_exec(&process, session->channel);
  session_destroy(session);

  session->state = NOP; // Finished

  return 0;
}

void grsd_handle_session(evutil_socket_t fd, short what, void* arg) {
  session_t session = (session_t)arg;
  ssh_message msg = ssh_message_get(session->session);
  int result;

  log_debug("Handle incoming data from session");

  if (ssh_message_type(msg) == -1) {
    log_debug("ssh_message_type of -1 received. Abort...");
    session_destroy(session);
    return;
  }

  // Depending on the session-state handle the ssh-data.
  // If the return-code is less than 0, the the handler asks to remove the
  // related event from the event_base_loop and destroy the session
  switch (session->state) {
    case AUTH:
      result = session_handle_auth(session, msg); break;
    case CHANNEL_OPEN:
      result = session_handle_channel_open(session, msg); break;
    case REQUEST_CHANNEL:
      result = session_handle_request_channel(session, msg); break;
    case NOP:
      result = -1; break;
  }

  ssh_message_free(msg);

  if (result < 0) {
    session_destroy(session);
  }
}

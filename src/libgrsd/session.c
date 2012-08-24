#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libssh/libssh.h>

#include "grsd.h"
#include "log.h"
#include "types.h"

session_t session_create(grsd_t handle) {
  struct _session* session;

  if (handle == NULL) {
    return NULL;
  }

  if ((session = malloc(sizeof(struct _session))) == NULL) {
    return NULL;
  }

  if ((session->session = ssh_new()) == NULL) {
    free(session);
    return NULL;
  }

  session->channel = NULL;
  session->session_ev = NULL;
  session->handle = handle;
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

  if (session->session_ev != NULL) {
    event_free(session->session_ev);
  }

  ssh_free(session->session);
  free(session);

  return 0;
}

grsd_t session_get_grsd(session_t session) {
  if (session == NULL) {
    return NULL;
  }

  return session->handle;
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

int session_accept(session_t session) {
  ssh_bind sshbind;

  if (session == NULL) {
    return -1;
  }

  sshbind = session->handle->bind;

  if (ssh_bind_accept(sshbind, session->session) != SSH_OK) {
    log_err("Error accepting connection: %s", ssh_get_error(sshbind));
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

int session_handle(session_t session) {
  ssh_message msg;
  int msg_type;

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

  ssh_message_free(msg);
  return 0;
}

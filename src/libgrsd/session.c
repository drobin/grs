#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libssh/libssh.h>

#include "itypes.h"
#include "log.h"
#include "session.h"

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

int session_accept(session_t session) {
  ssh_bind sshbind;
  
  if (session == NULL) {
    return -1;
  }
  
  sshbind = session->handle->bind;
  
  if (ssh_bind_accept(sshbind, session->session) != SSH_OK) {
    log_err("Error accepting connection: %s\n", ssh_get_error(sshbind));
    return -1;
  }
  
  if (ssh_handle_key_exchange(session->session) != SSH_OK) {
    log_err("Error in key exchange: %s\n", ssh_get_error(session->session));
    return -1;
  }
  
  log_info("Connection established");
  // TODO Who is connected?
  
  return 0;
}

static int session_handle_auth(session_t session, ssh_message msg) {
  char* user;
  char* password;
  
  log_debug("Handle authentication for session");
  
  if (ssh_message_type(msg) != SSH_REQUEST_AUTH) {
    log_debug("Ignoring message of type %i", ssh_message_type(msg));
    
    ssh_message_reply_default(msg);
    
    return 0;
  }
  
  if (ssh_message_subtype(msg) != SSH_AUTH_METHOD_PASSWORD) {
    log_debug("Currently only password-authentication is supported.");
    log_debug("Rejecting auth-type %i", ssh_message_subtype(msg));
    
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
  log_debug("Handle request for a channel");
  
  if (ssh_message_type(msg) != SSH_REQUEST_CHANNEL_OPEN) {
    log_debug("Ignoring message of type %i", ssh_message_type(msg));
    
    ssh_message_reply_default(msg);
    
    return 0;
  }
  
  session->channel = ssh_message_channel_request_open_reply_accept(msg);
  log_debug("Channel is open");
  
  session->state = NOP;
  
  return 0;
}

int session_handle(session_t session) {
  ssh_message msg;
  int result;
  
  msg = ssh_message_get(session->session);
  
  if (ssh_message_type(msg) == -1) {
    log_debug("ssh_message_type of -1 received. Abort...");
    return -1;
  }
  
  switch (session->state) {
  case AUTH: result = session_handle_auth(session, msg); break;
  case CHANNEL_OPEN: result = session_handle_channel_open(session, msg); break;
  case NOP: result = -1;
  }
  
  ssh_message_free(msg);
  
  return result;
}

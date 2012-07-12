#include <sys/errno.h>
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
  session->stdout_ev = NULL;
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
  
  if (session->stdout_ev != NULL) {
    event_free(session->stdout_ev);
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

#include <stdio.h>
#include <stdlib.h>

#include <libssh/libssh.h>

#include "itypes.h"
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
  
  session->handle = handle;
  
  return session;
}

int session_destroy(session_t session) {
  if (session == NULL) {
    return -1;
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
    printf("Error accepting connection: %s\n", ssh_get_error(sshbind));
    return -1;
  }
  
  return 0;
}
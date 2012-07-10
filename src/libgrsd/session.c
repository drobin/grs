#include <stdlib.h>

#include <libssh/libssh.h>

#include "session.h"

struct _session {
  grsd_t handle;
  ssh_session session;
};

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

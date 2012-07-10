#include <stdlib.h>

#include <libssh/libssh.h>

#include "session.h"

struct _session {
  ssh_session session;
};

session_t session_create() {
  struct _session* session;
  
  if ((session = malloc(sizeof(struct _session))) == NULL) {
    return NULL;
  }
  
  if ((session->session = ssh_new()) == NULL) {
    free(session);
    return NULL;
  }
  
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

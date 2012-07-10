#include <stdlib.h>

#include "session.h"

struct _session {
};

session_t session_create() {
  struct _session* session;
  
  if ((session = malloc(sizeof(struct _session))) == NULL) {
    return NULL;
  }
  
  return session;
}

int session_destroy(session_t session) {
  if (session == NULL) {
    return -1;
  }
  
  free(session);
  
  return 0;
}

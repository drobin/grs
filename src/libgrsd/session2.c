#include <stdlib.h>

#include "session2.h"

struct _session2 {
  enum session2_state state;
};

session2_t session2_create() {
  struct _session2* session;

  if ((session = malloc(sizeof(struct _session2))) == NULL) {
    return NULL;
  }

  session->state = NOOP;

  return session;
}

int session2_destroy(session2_t session) {
  if (session == NULL) {
    return -1;
  }

  free(session);

  return 0;
}

enum session2_state session2_get_state(session2_t session) {
  if (session == NULL) {
    return -1;
  }

  return session->state;
}

int session2_set_state(session2_t session, enum session2_state state) {
  if (session == NULL) {
    return -1;
  }

  session->state = state;

  return 0;
}

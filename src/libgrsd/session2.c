#include <stdlib.h>

#include "session2.h"

struct _session2 {
};

session2_t session2_create() {
  struct _session2* session;

  if ((session = malloc(sizeof(struct _session2))) == NULL) {
    return NULL;
  }

  return session;
}

int session2_destroy(session2_t session) {
  if (session == NULL) {
    return -1;
  }

  free(session);

  return 0;
}

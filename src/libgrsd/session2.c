#include <stdlib.h>
#include <string.h>

#include "session2.h"

struct _session2 {
  enum session2_state state;
  process_t process;
};

session2_t session2_create() {
  struct _session2* session;

  if ((session = malloc(sizeof(struct _session2))) == NULL) {
    return NULL;
  }

  session->state = NEED_AUTHENTICATION;
  session->process = NULL;

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

int session2_authenticate(session2_t session,
                           const char* username, const char* password) {

  if (session == NULL || username == NULL || password == NULL) {
    return -1;
  }

  if (session->state != NEED_AUTHENTICATION) {
    return -1;
  }

  if (strcmp(username, password) == 0) {
    session->state = NOOP;
    return 0;
  } else {
    return -1;
  }
}

process_t session2_get_process(session2_t session) {
  if (session == NULL) {
    return NULL;
  }

  return session->process;
}

int session2_set_process(session2_t session, process_t process) {
  if (session == NULL || process == NULL) {
    return -1;
  }

  session->process = process;

  return 0;
}

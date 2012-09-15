#include <stdlib.h>
#include <string.h>

#include "session.h"


struct _session {
  enum session_state state;
  process_t process;
};

session_t session_create() {
  struct _session* session;

  if ((session = malloc(sizeof(struct _session))) == NULL) {
    return NULL;
  }

  session->state = NEED_AUTHENTICATION;
  session->process = NULL;

  return session;
}

int session_destroy(session_t session) {
  if (session == NULL) {
    return -1;
  }

  if (session->process != NULL) {
    process_destroy(session->process);
  }

  free(session);

  return 0;
}

enum session_state session_get_state(session_t session) {
  if (session == NULL) {
    return -1;
  }

  return session->state;
}

int session_set_state(session_t session, enum session_state state) {
  if (session == NULL) {
    return -1;
  }

  session->state = state;

  return 0;
}

int session_authenticate(session_t session,
                         const char* username, const char* password) {

  if (session == NULL || username == NULL || password == NULL) {
    return -1;
  }

  if (session->state != NEED_AUTHENTICATION) {
    return -1;
  }

  if (strcmp(username, password) == 0) {
    session->state = NEED_PROCESS;
    return 0;
  } else {
    return -1;
  }
}

process_t session_get_process(session_t session) {
  if (session == NULL) {
    return NULL;
  }

  return session->process;
}

int session_set_process(session_t session, process_t process) {
  if (session == NULL || process == NULL) {
    return -1;
  }

  if (session->state != NEED_PROCESS) {
    return -1;
  }

  session->process = process;
  session->state = NEED_EXEC;

  return 0;
}

process_t session_create_process(session_t session, process_env_t env,
                                 const char* command) {
  if (session == NULL || env == NULL || command == NULL) {
    return NULL;
  }

  if (session->state != NEED_PROCESS) {
    return NULL;
  }

  session->process = process_prepare(env, command);
  session->state = NEED_EXEC;

  return session->process;
}

int session_exec(session_t session) {
  if (session == NULL) {
    return -1;
  }

  if (session->state != NEED_EXEC) {
    return -1;
  }

  process_exec(session->process);
  session->state = EXECUTING;

  return 0;
}

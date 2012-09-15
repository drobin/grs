#ifndef SESSION_H
#define SESSION_H

#include "process.h"

struct _session;
typedef struct _session* session_t;

enum session_state {
  NEED_AUTHENTICATION,
  NEED_PROCESS,
  NEED_EXEC,
  EXECUTING
};

session_t session_create();
int session_destroy(session_t session);

enum session_state session_get_state(session_t session);
int session_set_state(session_t session, enum session_state state);

int session_authenticate(session_t session,
                         const char* username, const char* password);
process_t session_get_process(session_t session);
int session_set_process(session_t session, process_t process);
process_t session_create_process(session_t session, process_env_t env,
                                 const char* command);

int session_exec(session_t session);

#endif  /* SESSION_H */

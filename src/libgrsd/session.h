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

session_t session2_create();
int session2_destroy(session_t session);

enum session_state session2_get_state(session_t session);
int session2_set_state(session_t session, enum session_state state);

int session2_authenticate(session_t session,
                          const char* username, const char* password);
process_t session2_get_process(session_t session);
int session2_set_process(session_t session, process_t process);

int session2_exec(session_t session);

#endif  /* SESSION_H */

#ifndef SESSION2_H
#define SESSION2_H

struct _session2;
typedef struct _session2* session2_t;

enum session2_state {
  NOOP
};

session2_t session2_create();
int session2_destroy(session2_t session);

enum session2_state session2_get_state(session2_t session);
int session2_set_state(session2_t session, enum session2_state state);

#endif  /* SESSION2_H */

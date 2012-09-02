#ifndef SESSION2_H
#define SESSION2_H

struct _session2;
typedef struct _session2* session2_t;

session2_t session2_create();
int session2_destroy(session2_t session);

#endif  /* SESSION2_H */

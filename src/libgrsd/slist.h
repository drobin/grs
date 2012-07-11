#ifndef SLIST_H
#define SLIST_H

#include "types.h"

struct _session_list;
typedef struct _session_list* slist_t;

slist_t slist_init();
int slist_destroy(slist_t slist);
int slist_get_size(slist_t slist);
int slist_prepend(slist_t slist, session_t session);

#endif  /* SLIST_H */
#ifndef SLIST_H
#define SLIST_H

#include "types.h"

struct _session_list;
typedef struct _session_list* slist_t;

slist_t slist_init();
int slist_destroy(slist_t slist);

#endif  /* SLIST_H */
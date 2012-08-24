#ifndef SLIST_H
#define SLIST_H

#include "session.h"

struct _session_list;
struct _session_list_iterator;

typedef struct _session_list* slist_t;
typedef struct _session_list_iterator* slist_it_t;

slist_t slist_init();
int slist_destroy(slist_t slist);
int slist_get_size(slist_t slist);
int slist_prepend(slist_t slist, session_t session);
int slist_remove(slist_t slist, session_t session);
int slist_clear(slist_t slist);

slist_it_t slist_iterator(slist_t slist);
int slist_iterator_destroy(slist_it_t it);
session_t slist_iterator_next(slist_it_t it);
int slist_iterator_remove(slist_it_t it);

#endif  /* SLIST_H */

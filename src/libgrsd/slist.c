#include <sys/queue.h>
#include <stdlib.h>

#include "slist.h"

struct slist_entry {
  session_t session;
  LIST_ENTRY(slist_entry) entries;
};

LIST_HEAD(_session_list, slist_entry);

slist_t slist_init() {
  struct _session_list* slist;
  
  if ((slist = malloc(sizeof(struct _session_list))) == NULL) {
    return NULL;
  }
  
  LIST_INIT(slist);
  
  return slist;
}

int slist_destroy(slist_t slist) {
  if (slist == NULL) {
    return -1;
  }
  
  free(slist);
  return 0;
}

int slist_get_size(slist_t slist) {
  if (slist == NULL) {
    return -1;
  }
  
  return 0;
}

int slist_prepend(slist_t slist, session_t session) {
  if (slist == NULL || session == NULL) {
    return -1;
  }
  
  return -2;
}

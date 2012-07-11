#include <sys/queue.h>
#include <stdlib.h>

#include "slist.h"

struct slist_entry {
  session_t session;
  LIST_ENTRY(slist_entry) entries;
};

LIST_HEAD(_slist_head, slist_entry);

struct _session_list {
  struct _slist_head head;
  unsigned int size;
};

struct _session_list_iterator {
  struct _slist_head* head;
  struct slist_entry* cur;
};

slist_t slist_init() {
  struct _session_list* slist;
  
  if ((slist = malloc(sizeof(struct _session_list))) == NULL) {
    return NULL;
  }
  
  LIST_INIT(&slist->head);
  slist->size = 0;
  
  return slist;
}

int slist_destroy(slist_t slist) {
  if (slist == NULL) {
    return -1;
  }
  
  slist_clear(slist);
  free(slist);
  
  return 0;
}

int slist_get_size(slist_t slist) {
  if (slist == NULL) {
    return -1;
  }
  
  return slist->size;
}

int slist_prepend(slist_t slist, session_t session) {
  struct slist_entry* entry;
  
  if (slist == NULL || session == NULL) {
    return -1;
  }
  
  if ((entry = malloc(sizeof(struct slist_entry))) == NULL) {
    return -1;
  }
  
  entry->session = session;
  
  LIST_INSERT_HEAD(&slist->head, entry, entries);
  slist->size++;
  
  return 0;
}

int slist_remove(slist_t slist, session_t session) {
  struct slist_entry* entry;
  
  if (slist == NULL || session == NULL) {
    return -1;
  }
  
  LIST_FOREACH(entry, &slist->head, entries) {
    if (entry->session == session) {
      LIST_REMOVE(entry, entries);
      slist->size--;
      free(entry);
      return 0;
    }
  }
  
  return -1;
}

int slist_clear(slist_t slist) {
  if (slist == NULL) {
    return -1;
  }
  
  while (!LIST_EMPTY(&slist->head)) {
    struct slist_entry* entry = LIST_FIRST(&slist->head);
    LIST_REMOVE(entry, entries);
    free(entry);
  }
  
  slist->size = 0;
  
  return 0;
}

slist_it_t slist_iterator(slist_t slist) {
  struct _session_list_iterator* it;
  
  if (slist == NULL) {
    return NULL;
  }
  
  if ((it = malloc(sizeof(struct _session_list_iterator))) == NULL) {
    return NULL;
  }
  
  it->head = &slist->head;
  it->cur = NULL;
  
  return it;
}

int slist_iterator_destroy(slist_it_t it) {
  if (it == NULL) {
    return -1;
  }
  
  free(it);
  
  return 0;
}

session_t slist_iterator_next(slist_it_t it) {
  if (it == NULL) {
    return NULL;
  }
  
  if (it->cur == NULL) {
    // Before first
    it->cur = LIST_FIRST(it->head);
  } else {
    it->cur = LIST_NEXT(it->cur, entries);
  }
  
  return (it->cur != NULL) ? it->cur->session : NULL;
}

// FIXME This is awful. You need to switch to another list-type, which supports
//        backward-traversion
static struct slist_entry* slist_previous(struct _slist_head* head,
                                          struct slist_entry* entry) {
  struct slist_entry* prev = LIST_FIRST(head);
  
  while (prev != NULL && LIST_NEXT(prev, entries) != entry) {
    prev = LIST_NEXT(prev, entries);
  }
  
  return prev;
}

int slist_iterator_remove(slist_it_t it) {
  struct slist_entry* prev;
  
  if (it == NULL) {
    return -1;
  }

  if (it->cur == NULL) {
    // The iterator does not point on an entry
    return -1;
  }

  prev = slist_previous(it->head, it->cur);
  
  LIST_REMOVE(it->cur, entries);
  free(it->cur);
  it->cur = prev;
  
  return 0;
}
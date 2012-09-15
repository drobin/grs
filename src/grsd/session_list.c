#include <stdlib.h>
#include <string.h>

#include "session_list.h"

struct session_entry* session_list_prepend(
  struct session_list* list, ssh_session session) {

  struct session_entry* entry;

  if (list == NULL || session == NULL) {
    return NULL;
  }

  if ((entry = malloc(sizeof(struct session_entry))) == NULL) {
    return NULL;
  }

  memset(entry, 0, sizeof(struct session_entry));
  entry->session = session;
  LIST_INSERT_HEAD(&(list->head), entry, entries);

  return entry;
}

int session_list_remove(struct session_list* list,
                        struct session_entry* entry) {
  if (list == NULL || entry == NULL) {
    return -1;
  }

  LIST_REMOVE(entry, entries);
  free(entry);

  return 0;
}

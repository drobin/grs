#include <stdlib.h>
#include <string.h>

#include "session_list.h"

struct session_entry* session_list_prepend(
  struct session_head* head, ssh_session session) {

  struct session_entry* entry;

  if (head == NULL || session == NULL) {
    return NULL;
  }

  if ((entry = malloc(sizeof(struct session_entry))) == NULL) {
    return NULL;
  }

  memset(entry, 0, sizeof(struct session_entry));
  entry->session = session;
  LIST_INSERT_HEAD(head, entry, entries);

  return entry;
}

int session_list_remove(struct session_entry* entry) {
  if (entry == NULL) {
    return -1;
  }

  LIST_REMOVE(entry, entries);
  free(entry);

  return 0;
}

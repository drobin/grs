#ifndef SESSION_LIST_H
#define SESSION_LIST_H

#include <sys/queue.h>

#include <libssh/libssh.h>

struct session_entry {
  ssh_session session;
  LIST_ENTRY(session_entry) entries;
};

LIST_HEAD(session_head, session_entry);

struct session_entry* session_list_prepend(
  struct session_head* head, ssh_session session);
int session_list_remove(struct session_entry* entry);

#endif  /* SESSION_LIST_H */

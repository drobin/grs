#ifndef SESSION_LIST_H
#define SESSION_LIST_H

#include <sys/queue.h>

#include <libssh/libssh.h>
#include <process.h>
#include <session.h>

struct session_entry {
  ssh_session session;
  ssh_channel channel;
  session_t grs_session;
  LIST_ENTRY(session_entry) entries;
};

LIST_HEAD(session_head, session_entry);

struct session_list {
  struct session_head head;
  int size;
  char buffer[2048];
};

#define SESSION_LIST_INIT(l) { \
  memset(&l, 0, sizeof(struct session_list)); \
  LIST_INIT(&(l.head)); \
}

#define SESSION_LIST_FOREACH(e, l) \
  LIST_FOREACH(e, &(l.head), entries)

struct session_entry* session_list_prepend(
  struct session_list* list, ssh_session session);
int session_list_remove(struct session_list* list,
                        struct session_entry* entry);

#endif  /* SESSION_LIST_H */

#ifndef SESSION_LIST_H
#define SESSION_LIST_H

#include <sys/queue.h>

#include <libssh/libssh.h>
#include <process.h>
#include <session2.h>

struct session_entry {
  ssh_session session;
  ssh_channel channel;
  session2_t grs_session;
  process_env_t env;
  process_t process;
  LIST_ENTRY(session_entry) entries;
};

LIST_HEAD(session_head, session_entry);

struct session_list {
  struct session_head head;
};

#define SESSION_LIST_INIT(l) { \
  memset(&l, 0, sizeof(struct session_list)); \
  LIST_INIT(&(l.head)); \
}

struct session_entry* session_list_prepend(
  struct session_head* head, ssh_session session);
int session_list_remove(struct session_entry* entry);

#endif  /* SESSION_LIST_H */

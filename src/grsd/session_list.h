/*******************************************************************************
 *
 * This file is part of grs.
 *
 * grs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * grs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with grs.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

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

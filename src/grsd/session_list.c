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
  list->size++;

  return entry;
}

int session_list_remove(struct session_list* list,
                        struct session_entry* entry) {
  if (list == NULL || entry == NULL) {
    return -1;
  }

  LIST_REMOVE(entry, entries);
  list->size--;
  free(entry);

  return 0;
}

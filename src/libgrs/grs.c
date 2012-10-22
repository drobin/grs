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

#include <sys/queue.h>
#include <stdlib.h>
#include <string.h>

#include "grs.h"

struct command_entry {
  char* command;
  struct command_hooks hooks;
  struct command_entry* parent;
  LIST_ENTRY(command_entry) entries;
};

LIST_HEAD(command_head, command_entry);

struct _grs {
  acl_t acl;
  struct command_head cmd_head;
};

static struct command_entry* get_command_entry(struct _grs* handle,
                                               const char* command,
                                               struct command_entry* parent,
                                               int create) {
  struct command_entry* entry;

  LIST_FOREACH(entry, &(handle->cmd_head), entries) {
    if (entry->parent == parent && strcmp(entry->command, command) == 0) {
      return entry;
    }
  }

  if (create) {
    if ((entry = malloc(sizeof(struct command_entry))) == NULL) {
      return NULL;
    }

    entry->command = strdup(command);
    memset(&entry->hooks, 0, sizeof(struct command_hooks));
    entry->parent = parent;
    LIST_INSERT_HEAD(&(handle->cmd_head), entry, entries);

    return entry;
  } else {
    return NULL;
  }
}

grs_t grs_init() {
  struct _grs* handle;

  if ((handle = malloc(sizeof(struct _grs))) == NULL) {
    return NULL;
  }

  memset(handle, 0, sizeof(struct _grs));

  if ((handle->acl = acl_init()) == NULL) {
    free(handle);
  }

  LIST_INIT(&handle->cmd_head);

  return handle;
}

int grs_destroy(grs_t handle) {
  struct command_entry* entry;

  if (handle == NULL) {
    return -1;
  }

  acl_destroy(handle->acl);

  while ((entry = LIST_FIRST(&(handle->cmd_head))) != NULL) {
    LIST_REMOVE(entry, entries);
    free(entry->command);
    free(entry);
  }

  free(handle);

  return 0;
}

acl_t grs_get_acl(grs_t handle) {
  if (handle == NULL) {
    return NULL;
  }

  return handle->acl;
}

int grs_register_command(grs_t handle, char *const command[],
                         struct command_hooks* hooks) {
  struct command_entry* entry;
  struct command_entry* parent;
  int i;

  if (handle == NULL || command == NULL || command[0] == NULL ||
      hooks == NULL || hooks->exec == NULL) {
    return -1;
  }

  for (parent = NULL, i = 0; command[i] != NULL; i++) {
    entry = get_command_entry(handle, command[i], parent, 1);
    parent = entry;
  }

  if (entry->hooks.exec == NULL || entry->hooks.exec == hooks->exec) {
    memcpy(&entry->hooks, hooks, sizeof(struct command_hooks));
    return 0;
  } else {
    return -1;
  }
}

struct command_hooks* grs_get_command_hooks(grs_t handle,
                                            char *const command[]) {
  struct command_entry* entry;
  struct command_entry* parent;
  struct command_entry* result;
  int i;

  if (handle == NULL || command == NULL || command[0] == NULL) {
    return NULL;
  }

  for (result = NULL, parent = NULL, i = 0; command[i] != NULL; i++) {
    if ((entry = get_command_entry(handle, command[i], parent, 0)) != NULL) {
      result = entry;
      parent = entry;
    } else {
      break;
    }
  }

  if (result == NULL || result->hooks.exec == NULL) {
    return NULL;
  }

  return &result->hooks;
}

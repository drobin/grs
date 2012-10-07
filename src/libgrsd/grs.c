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
  command2_hook hook;
  LIST_ENTRY(command_entry) entries;
};

LIST_HEAD(command_head, command_entry);

struct _grs {
  acl_t acl;
  struct command_head cmd_head;
  process_env_t process_env;
};

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

  if ((handle->process_env = process_env_create()) == NULL) {
    acl_destroy(handle->acl);
    free(handle);
  }

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

  process_env_destroy(handle->process_env);
  free(handle);

  return 0;
}

acl_t grs_get_acl(grs_t handle) {
  if (handle == NULL) {
    return NULL;
  }

  return handle->acl;
}

int grs_register_command(grs_t handle, const char* command, command2_hook hook) {
  struct command_entry* entry;

  if (handle == NULL || command == NULL || hook == NULL) {
    return -1;
  }

  if ((entry = malloc(sizeof(struct command_entry))) == NULL) {
    return -1;
  }

  entry->command = strdup(command);
  entry->hook = hook;
  LIST_INSERT_HEAD(&(handle->cmd_head), entry, entries);

  return 0;
}

command2_hook grs_get_command(grs_t handle, const char* command) {
  struct command_entry* entry;

  if (handle == NULL || command == NULL) {
    return NULL;
  }

  LIST_FOREACH(entry, &(handle->cmd_head), entries) {
    if (strcmp(entry->command, command) == 0) {
      return entry->hook;
    }
  }

  return NULL;
}

process_env_t grs_get_process_env(grs_t handle) {
  if (handle == NULL) {
    return NULL;
  }

  return handle->process_env;
}

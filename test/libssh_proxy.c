#include <sys/queue.h>
#include <stdlib.h>
#include <string.h>

#include "libssh_proxy.h"

struct libssh_proxy_option {
  const char* option;

  union {
    int int_val;
    struct list_head* list_val;
    int* intptr_val;
  } v;

  LIST_ENTRY(libssh_proxy_option) entries;
};

LIST_HEAD(_libssh_proxy_option_head, libssh_proxy_option);

struct libssh_proxy_func {
  const char* func;
  struct _libssh_proxy_option_head options_head;
  LIST_ENTRY(libssh_proxy_func) entries;
};

LIST_HEAD(_libssh_proxy_head, libssh_proxy_func);

static struct libssh_proxy_func* find_func(const char* func, int create) {
  struct libssh_proxy_func* entry;

  LIST_FOREACH(entry, libssh_proxy_env, entries) {
    if (strcmp(entry->func, func) == 0) {
      return entry;
    }
  }

  if (create) {
    entry = malloc(sizeof(struct libssh_proxy_func));
    entry->func = func;
    LIST_INIT(&entry->options_head);
    LIST_INSERT_HEAD(libssh_proxy_env, entry, entries);
  } else {
    entry = NULL;
  }

  return entry;
}

static struct libssh_proxy_option* find_option(struct libssh_proxy_func* func,
                                               const char* option, int create) {
  struct libssh_proxy_option* entry;

  LIST_FOREACH(entry, &func->options_head, entries) {
    if (strcmp(entry->option, option) == 0) {
      return entry;
    }
  }

  if (create) {
    entry = malloc(sizeof(struct libssh_proxy_option));
    entry->option = option;
    LIST_INSERT_HEAD(&func->options_head, entry, entries);
  } else {
    entry = NULL;
  }

  return entry;
}

int libssh_proxy_init() {
  if ((libssh_proxy_env = malloc(sizeof(struct _libssh_proxy_head))) == NULL) {
    return -1;
  }

  LIST_INIT(libssh_proxy_env);

  return 0;
}

int libssh_proxy_destroy() {
  while (!LIST_EMPTY(libssh_proxy_env)) {
    struct libssh_proxy_func* entry = LIST_FIRST(libssh_proxy_env);

    while (!LIST_EMPTY(&entry->options_head)) {
      struct libssh_proxy_option* oentry = LIST_FIRST(&entry->options_head);
      LIST_REMOVE(oentry, entries);
      free(oentry);
    }

    LIST_REMOVE(entry, entries);
    free(entry);
  }

  free(libssh_proxy_env);

  return 0;
}

int libssh_proxy_get_option_int(const char* func, const char* option, int def) {
  struct libssh_proxy_func* func_entry = find_func(func, 0);

  if (func_entry != NULL) {
    struct libssh_proxy_option* option_entry;

    option_entry = find_option(func_entry, option, 0);
    return (option_entry != NULL) ? option_entry->v.int_val : def;
  } else {
    return def;
  }
}

int libssh_proxy_set_option_int(const char* func, const char* option, int val) {
  struct libssh_proxy_func* func_entry = find_func(func, 1);
  struct libssh_proxy_option* option_entry = find_option(func_entry, option, 1);

  option_entry->v.int_val = val;

  return 0;
}

struct list_head* libssh_proxy_get_option_list(const char* func,
                                               const char* option,
                                               struct list_head* def) {
  struct libssh_proxy_func* func_entry = find_func(func, 0);

  if (func_entry != NULL) {
    struct libssh_proxy_option* option_entry;

    option_entry = find_option(func_entry, option, 0);
    return (option_entry != NULL) ? option_entry->v.list_val : def;
  } else {
    return def;
  }
}

int libssh_proxy_set_option_list(const char* func,
                                 const char* option,
                                 struct list_head* val) {
  struct libssh_proxy_func* func_entry = find_func(func, 1);
  struct libssh_proxy_option* option_entry = find_option(func_entry, option, 1);

  option_entry->v.list_val = val;

  return 0;
}

int* libssh_proxy_get_option_intptr(const char* func, const char* option, int* def) {
  struct libssh_proxy_func* func_entry = find_func(func, 0);

  if (func_entry != NULL) {
    struct libssh_proxy_option* option_entry;

    option_entry = find_option(func_entry, option, 0);
    return (option_entry != NULL) ? option_entry->v.intptr_val : def;
  } else {
    return def;
  }
}

int libssh_proxy_set_option_intptr(const char* func, const char* option, int* val) {
  struct libssh_proxy_func* func_entry = find_func(func, 1);
  struct libssh_proxy_option* option_entry = find_option(func_entry, option, 1);

  option_entry->v.intptr_val = val;

  return 0;
}

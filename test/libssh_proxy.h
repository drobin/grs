#ifndef LIBSSH_PROXY_H
#define LIBSSH_PROXY_H

#include <sys/queue.h>

struct _libssh_proxy_head;
typedef struct _libssh_proxy_head* libssh_proxy_t;

libssh_proxy_t libssh_proxy_env;

struct list_entry {
  union {
    int int_val;
  } v;

  LIST_ENTRY(list_entry) entries;
};

LIST_HEAD(list_head, list_entry);

int libssh_proxy_make_list(struct list_head* head,
                           struct list_entry* entries,
                           int nentries);

int libssh_proxy_init();
int libssh_proxy_destroy();

int libssh_proxy_get_option_int(const char* func, const char* option, int def);
int libssh_proxy_set_option_int(const char* func, const char* option, int val);

struct list_head* libssh_proxy_get_option_list(const char* func,
                                               const char* option,
                                               struct list_head* def);
int libssh_proxy_set_option_list(const char* func,
                                 const char* option,
                                 struct list_head* val);

int* libssh_proxy_get_option_intptr(const char* func, const char* option, int* def);
int libssh_proxy_set_option_intptr(const char* func, const char* option, int* val);

#endif  /* LIBSSH_PROXY_H */

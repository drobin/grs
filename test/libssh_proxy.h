#ifndef LIBSSH_PROXY_H
#define LIBSSH_PROXY_H

#include <sys/queue.h>

#include <libssh/libssh.h>

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

char* libssh_proxy_get_option_string(const char* func, const char* option,
                                     char* def);
int libssh_proxy_set_option_string(const char* func, const char* option,
                                   char* val);

struct list_head* libssh_proxy_get_option_list(const char* func,
                                               const char* option,
                                               struct list_head* def);
int libssh_proxy_set_option_list(const char* func,
                                 const char* option,
                                 struct list_head* val);

int* libssh_proxy_get_option_intptr(const char* func, const char* option, int* def);
int libssh_proxy_set_option_intptr(const char* func, const char* option, int* val);

int libssh_proxy_channel_get_size(ssh_channel channel);
char* libssh_proxy_channel_get_data(ssh_channel channel);
int libssh_proxy_channel_set_data(ssh_channel channel, char* data, int len);

#endif  /* LIBSSH_PROXY_H */

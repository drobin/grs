#ifndef LIBSSH_PROXY_H
#define LIBSSH_PROXY_H

struct _libssh_proxy_head;
typedef struct _libssh_proxy_head* libssh_proxy_t;

libssh_proxy_t libssh_proxy_env;

int libssh_proxy_init();
int libssh_proxy_destroy();

int libssh_proxy_get_option_int(const char* func, const char* option, int def);
int libssh_proxy_set_option_int(const char* func, const char* option, int val);
int* libssh_proxy_get_option_intptr(const char* func, const char* option, int* def);
int libssh_proxy_set_option_intptr(const char* func, const char* option, int* val);

#endif  /* LIBSSH_PROXY_H */

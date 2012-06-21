#ifndef LIBSSH_PROXY_H
#define LIBSSH_PROXY_H

struct libssh_proxy_env {

};

struct libssh_proxy_env* ssh_proxy_env;

int libssh_proxy_init();
void libssh_proxy_destroy();

#endif /* LIBSSH_PROXY */

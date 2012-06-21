#ifndef LIBSSH_PROXY_H
#define LIBSSH_PROXY_H

struct libssh_proxy_env {

};

int libssh_proxy_init(struct libssh_proxy_env* env);
void libssh_proxy_destroy(struct libssh_proxy_env* env);

#endif /* LIBSSH_PROXY */

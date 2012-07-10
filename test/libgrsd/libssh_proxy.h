#ifndef LIBSSH_PROXY_H
#define LIBSSH_PROXY_H

struct libssh_proxy_env {
  int ssh_bind_new_should_fail;
  int ssh_bind_options_set_should_fail;
  int ssh_bind_listen_should_fail;
  int ssh_new_should_fail;
};

struct libssh_proxy_env* ssh_proxy_env;

int libssh_proxy_init();
void libssh_proxy_destroy();
int libssh_proxy_connect();

#endif /* LIBSSH_PROXY */

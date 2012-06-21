#include <stdlib.h>
#include <string.h>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "libssh_proxy.h"

int libssh_proxy_init() {
  if (ssh_proxy_env != NULL) {
    libssh_proxy_destroy();
  }

  if ((ssh_proxy_env = malloc(sizeof(struct libssh_proxy_env))) == NULL) {
    return -1;
  }

  memset(ssh_proxy_env, 0, sizeof(struct libssh_proxy_env));

  return 0;
}

void libssh_proxy_destroy() {
  free(ssh_proxy_env);
  ssh_proxy_env = NULL;
}

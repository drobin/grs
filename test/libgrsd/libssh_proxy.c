#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "libssh_proxy.h"

struct ssh_bind_struct {
  int so;
};

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

ssh_bind ssh_bind_new() {
  struct ssh_bind_struct* sshbind;
  int optval;

  if (ssh_proxy_env->ssh_bind_new_should_fail) {
    return NULL;
  }

  if ((sshbind = malloc(sizeof(struct ssh_bind_struct))) == NULL) {
    return NULL;
  }

  if ((sshbind->so = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    perror(__FUNCTION__);
    free(sshbind);
    return NULL;
  }
  
  optval = 1;
  if (setsockopt(sshbind->so, SOL_SOCKET, SO_REUSEADDR,
                 &optval, sizeof(optval)) == -1) {
    perror(__FUNCTION__);
    close(sshbind->so);
    free(sshbind);
    return NULL;
  }
  
  return sshbind;
}

void ssh_bind_free(ssh_bind sshbind) {
  close(sshbind->so);
  free(sshbind);
}

int ssh_bind_options_set(ssh_bind sshbind, enum ssh_bind_options_e type,
  const void* value) {

  return ssh_proxy_env->ssh_bind_options_set_should_fail ? SSH_ERROR : SSH_OK;
}

int ssh_bind_listen(ssh_bind ssh_bind_o) {
  struct sockaddr_in addr;
  
  if (ssh_proxy_env->ssh_bind_listen_should_fail) {
    return -1;
  }
  
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(5000);
  addr.sin_addr.s_addr = INADDR_ANY;
  
  if (bind(ssh_bind_o->so, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror(__FUNCTION__);
    return -1;
  }
  
  if (listen(ssh_bind_o->so, 128) == -1) {
    perror(__FUNCTION__);
    return -1;
  }
  
  return 0;
}

const char* ssh_get_error(void *error) {
  return NULL;
}

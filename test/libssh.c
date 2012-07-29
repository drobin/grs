#include <stdlib.h>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "libssh_proxy.h"

struct ssh_session_struct {
};

int ssh_bind_accept(ssh_bind ssh_bind_o, ssh_session session) {
  if (libssh_proxy_get_option_int("ssh_bind_accept", "fail", 0)) {
    return SSH_ERROR;
  } else {
    return SSH_OK;
  }
}

void ssh_bind_free(ssh_bind ssh_bind_o) {
  // TODO Needs to be implemented
}

socket_t ssh_bind_get_fd(ssh_bind ssh_bind_o) {
  // TODO Needs to be implemented
  return 23;
}

int ssh_bind_listen(ssh_bind ssh_bind_o) {
  // TODO Needs to be implemented
  return 23;
}

ssh_bind ssh_bind_new(void) {
  // TODO Needs to be implemented
  return NULL;
}

int ssh_bind_options_set(ssh_bind sshbind, enum ssh_bind_options_e type,
                         const void *value) {
  // TODO Needs to be implemented
  return 23;
}

void ssh_channel_free(ssh_channel channel) {
  // TODO Needs to be implemented
}

ssh_session ssh_channel_get_session(ssh_channel channel) {
  // TODO Needs to be implemented
  return NULL;
}

int ssh_channel_read(ssh_channel channel, void *dest, uint32_t count,
                     int is_stderr) {
  // TODO Needs to be implemented
  return 23;
}

int ssh_channel_send_eof(ssh_channel channel) {
  // TODO Needs to be implemented
  return 23;
}

int ssh_channel_write(ssh_channel channel, const void *data, uint32_t len) {
  // TODO Needs to be implemented
  return 23;
}

void ssh_free(ssh_session session) {
  free(session);
}

const char *ssh_get_error(void *error) {
  // TODO Needs to be implemented
  return NULL;
}

socket_t ssh_get_fd(ssh_session session) {
  // TODO Needs to be implemented
  return 23;
}

int ssh_handle_key_exchange(ssh_session session) {
  if (libssh_proxy_get_option_int("ssh_handle_key_exchange", "fail", 0)) {
    return SSH_ERROR;
  } else {
    return SSH_OK;
  }
}

char *ssh_message_auth_password(ssh_message msg) {
  // TODO Needs to be implemented
  return NULL;
}

int ssh_message_auth_reply_success(ssh_message msg, int partial) {
  // TODO Needs to be implemented
  return 23;
}

int ssh_message_auth_set_methods(ssh_message msg, int methods) {
  // TODO Needs to be implemented
  return 23;
}

char *ssh_message_auth_user(ssh_message msg) {
  // TODO Needs to be implemented
  return NULL;
}

char *ssh_message_channel_request_command(ssh_message msg) {
  // TODO Needs to be implemented
  return NULL;
}

ssh_channel ssh_message_channel_request_open_reply_accept(ssh_message msg) {
  // TODO Needs to be implemented
  return NULL;
}

int ssh_message_channel_request_reply_success(ssh_message msg) {
  // TODO Needs to be implemented
  return 23;
}

void ssh_message_free(ssh_message msg) {
  // TODO Needs to be implemented
}

ssh_message ssh_message_get(ssh_session session) {
  // TODO Needs to be implemented
  return NULL;
}

int ssh_message_reply_default(ssh_message msg) {
  // TODO Needs to be implemented
  return 23;
}

int ssh_message_subtype(ssh_message msg) {
  // TODO Needs to be implemented
  return 23;
}

int ssh_message_type(ssh_message msg) {
  // TODO Needs to be implemented
  return 23;
}

ssh_session ssh_new(void) {
  if (libssh_proxy_get_option_int("ssh_new", "fail", 0)) {
    return NULL;
  } else {
    return malloc(sizeof(struct ssh_session_struct));
  }
}

int ssh_select(ssh_channel *channels, ssh_channel *outchannels, socket_t maxfd,
               fd_set *readfds, struct timeval *timeout) {
  // TODO Needs to be implemented
  return 23;
}

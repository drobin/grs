#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "libssh_proxy.h"

struct ssh_bind_struct {
};

struct ssh_session_struct {
};

struct ssh_channel_struct {
  char* buf;
  int bufsize;
};

struct ssh_message_struct {
};

int libssh_proxy_channel_get_size(ssh_channel channel) {
  return channel->bufsize;
}

char* libssh_proxy_channel_get_data(ssh_channel channel) {
  return channel->buf;
}

int libssh_proxy_channel_set_data(ssh_channel channel, char* data, int len) {
  free(channel->buf);
  channel->buf = malloc(len);

  memcpy(channel->buf, data, len);
  channel->bufsize = len;

  return 0;
}

int ssh_bind_accept(ssh_bind ssh_bind_o, ssh_session session) {
  if (libssh_proxy_get_option_int("ssh_bind_accept", "fail", 0)) {
    return SSH_ERROR;
  } else {
    return SSH_OK;
  }
}

void ssh_bind_free(ssh_bind ssh_bind_o) {
  free(ssh_bind_o);
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
  return malloc(sizeof(struct ssh_bind_struct));
}

int ssh_bind_options_set(ssh_bind sshbind, enum ssh_bind_options_e type,
                         const void *value) {
  // TODO Needs to be implemented
  return 23;
}

void ssh_channel_free(ssh_channel channel) {
  free(channel->buf);
  free(channel);
}

ssh_session ssh_channel_get_session(ssh_channel channel) {
  // TODO Needs to be implemented
  return NULL;
}

int ssh_channel_read(ssh_channel channel, void *dest, uint32_t count,
                     int is_stderr) {
  int nbytes = (count > channel->bufsize) ? channel->bufsize : count;

  if (nbytes == 0) {
    return 0;
  }

  memcpy(dest, channel->buf, nbytes);
  memmove(channel->buf, channel->buf + nbytes, channel->bufsize - nbytes);
  channel->bufsize -= nbytes;

  return nbytes;
}

int ssh_channel_send_eof(ssh_channel channel) {
  // TODO Needs to be implemented
  return 23;
}

int ssh_channel_write(ssh_channel channel, const void *data, uint32_t len) {
  channel->buf = realloc(channel->buf, channel->bufsize + len);
  memcpy(channel->buf + channel->bufsize, data, len);
  channel->bufsize += len;

  return len;
}

void ssh_free(ssh_session session) {
  free(session);
}

const char *ssh_get_error(void *error) {
  // TODO Needs to be implemented
  return NULL;
}

socket_t ssh_get_fd(ssh_session session) {
  return libssh_proxy_get_option_int("ssh_get_fd", "fd", -1);
}

int ssh_handle_key_exchange(ssh_session session) {
  if (libssh_proxy_get_option_int("ssh_handle_key_exchange", "fail", 0)) {
    return SSH_ERROR;
  } else {
    return SSH_OK;
  }
}

int ssh_is_connected(ssh_session session) {
  return libssh_proxy_get_option_int("ssh_is_connected", "connected", 1);
}

char *ssh_message_auth_password(ssh_message msg) {
  return libssh_proxy_get_option_string(
    "ssh_message_auth_password", "result", NULL);
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
  return libssh_proxy_get_option_string(
    "ssh_message_auth_user", "result", NULL);
}

char *ssh_message_channel_request_command(ssh_message msg) {
  // TODO Needs to be implemented
  return NULL;
}

ssh_channel ssh_message_channel_request_open_reply_accept(ssh_message msg) {
  if (libssh_proxy_get_option_int(
      "ssh_message_channel_request_open_reply_accept", "fail", 0)) {
    return NULL;
  } else {
    struct ssh_channel_struct* channel;

    channel = malloc(sizeof(struct ssh_channel_struct));
    memset(channel, 0, sizeof(struct ssh_channel_struct));

    return channel;
  }
}

int ssh_message_channel_request_reply_success(ssh_message msg) {
  // TODO Needs to be implemented
  return 23;
}

void ssh_message_free(ssh_message msg) {
  free(msg);
}

ssh_message ssh_message_get(ssh_session session) {
  if (libssh_proxy_get_option_int("ssh_message_get", "fail", 0)) {
    return NULL;
  } else {
    return malloc(sizeof(struct ssh_message_struct));
  }
}

int ssh_message_reply_default(ssh_message msg) {
  // TODO Needs to be implemented
  return 23;
}

int ssh_message_subtype(ssh_message msg) {
  struct list_head* head =
    libssh_proxy_get_option_list("ssh_message_subtype", "results", NULL);
  struct list_entry* entry;

  if (head == NULL) {
    fprintf(stderr, "%s: No options assigned\n", __FUNCTION__);
    return -1;
  }

  if ((entry = LIST_FIRST(head)) == NULL) {
    fprintf(stderr, "%s: The list is empty\n", __FUNCTION__);
    return -1;
  }

  LIST_REMOVE(entry, entries);

  return entry->v.int_val;
}

int ssh_message_type(ssh_message msg) {
  struct list_head* head =
    libssh_proxy_get_option_list("ssh_message_type", "results", NULL);
  struct list_entry* entry;

  if (head == NULL) {
    fprintf(stderr, "%s: No options assigned\n", __FUNCTION__);
    return -1;
  }

  if ((entry = LIST_FIRST(head)) == NULL) {
    fprintf(stderr, "%s: The list is empty\n", __FUNCTION__);
    return -1;
  }

  LIST_REMOVE(entry, entries);

  return entry->v.int_val;
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
  if (libssh_proxy_get_option_int("ssh_select", "readfds", 0)) {
    return select(maxfd, readfds, NULL, NULL, timeout);
  } else {
    // TODO Needs to be implemented
    return 23;
  }
}

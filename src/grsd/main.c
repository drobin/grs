/*******************************************************************************
 *
 * This file is part of grs.
 *
 * grs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * grs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with grs.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <sys/errno.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libgrs/core/acl.h>
#include <libgrs/core/grs.h>
#include <libgrs/core/log.h>
#include <extension/command.h>
#include <libssh/libssh.h>
#include <libssh/server.h>

#include "io.h"
#include "session_list.h"

/**
 * Flag is set to true if you need to leave the main selection loop.
 */
static int leave_selection_loop = 0;

static void leave_handler(int sig) {
  log_debug("Ask to leave selection loop");
  leave_selection_loop = 1;
}

static void usage() {
  printf("USAGE\n");
  exit(1);
}

static void setup_acl(grs_t grs, int writable) {
  acl_t acl;
  acl_node_t node;
  struct acl_node_value* value;

  acl = grs_get_acl(grs);
  node = acl_get_root_node(acl);
  value = acl_node_get_value(node, 1);
  value->flag = 1; // Default: you have access

  if (!writable) {
    const char* path = "git-receive-pack";
    node = acl_get_node(acl, &path, 1);
    value = acl_node_get_value(node, 1);
    value->flag = 0; // Forbit git-receive-pack if read-only
  }
}

static void close_and_free_session_entry(struct session_list* list,
                                         struct session_entry* entry) {
  if (entry->channel != NULL) {
    ssh_channel_close(entry->channel);
    ssh_channel_free(entry->channel);
  }

  ssh_free(entry->session);
  session_destroy(entry->grs_session);
  session_list_remove(list, entry);
}

static int handle_ssh_authentication(session_t session, ssh_message msg) {
  char* user;
  char* password;

  log_debug("Handle authentication for session");

  if (ssh_message_subtype(msg) != SSH_AUTH_METHOD_PASSWORD) {
    log_debug("Currently only password-authentication is supported.");
    log_debug("Rejecting auth-type %i", ssh_message_subtype(msg));

    ssh_message_auth_set_methods(msg, SSH_AUTH_METHOD_PASSWORD);
    ssh_message_reply_default(msg);

    return 0;
  }

  user = ssh_message_auth_user(msg);
  password = ssh_message_auth_password(msg);
  log_debug("Authentication requested with %s/%s", user, password);


  if (session_authenticate(session, user, password) == 0) {
    log_debug("Authentication succeeded");
    ssh_message_auth_reply_success(msg, 0);
  } else {
    log_debug("Authentication rejected");
    ssh_message_reply_default(msg);
  }

  return 0;
}

static int handle_ssh_channel_open(struct session_entry* entry, ssh_message msg) {
  log_debug("Handle open-request for a channel");

  entry->channel = ssh_message_channel_request_open_reply_accept(msg);
  if (entry->channel != NULL) {
    log_debug("Channel is open");

    return 0;
  } else {
    log_err("Failed to open channel: %s", ssh_get_error(entry->session));
    return -1;
  }
}

static int handle_ssh_channel_request(struct session_entry* entry,
                                      ssh_message msg, grs_t grs) {
  log_debug("Handle channel-request");

  if (ssh_message_subtype(msg) != SSH_CHANNEL_REQUEST_EXEC) {
    log_debug("Ignoring channel-type %i", ssh_message_subtype(msg));

    ssh_message_reply_default(msg);

    return 0;
  }

  ssh_message_channel_request_reply_success(msg);
  log_debug("Channel request accepted");

  session_set_command(entry->grs_session,
                      ssh_message_channel_request_command(msg));

  return 0;
}

static int handle_ssh_session(struct session_list* list,
                              struct session_entry* entry, grs_t grs) {
  int result = 0;
  ssh_message msg;
  int msg_type;

  log_debug("Session is selected");

  if ((msg = ssh_message_get(entry->session)) == NULL) {
    log_err("Failed to read message from session: %s",
            ssh_get_error(entry->session));

    close_and_free_session_entry(list, entry);

    return -1;
  }

  switch ((msg_type = ssh_message_type(msg))) {
    case SSH_REQUEST_AUTH:
      log_debug("Session state: handle authentication");
      result = handle_ssh_authentication(entry->grs_session, msg);
      break;
    case SSH_REQUEST_CHANNEL_OPEN:
      log_debug("Session state: open a channel");
      result = handle_ssh_channel_open(entry, msg);
      break;
    case SSH_REQUEST_CHANNEL:
      log_debug("Session state: channel request");
      result = handle_ssh_channel_request(entry, msg, grs);
      break;
    default:
      // Ignore message
      log_debug("Ignore message of type %i", msg_type);
      ssh_message_reply_default(msg);
      result = 0;
      break;
  }

  if (result != 0) {
    log_debug("Asked to close and free the session");
    close_and_free_session_entry(list, entry);
  }

  ssh_message_free(msg);

  return 0;
}

static int handle_ssh_bind(ssh_bind bind, struct session_list* slist,
                           grs_t grs) {
  struct session_entry* entry;
  ssh_session session;
  session_t grs_session;
  int result;

  log_debug("SSH server bind selected");

  if ((session = ssh_new()) != NULL) {
    log_debug("SSH session created");
  } else {
    log_err("Failed to create a new SSH session");
    return -1;
  }

  if ((result = ssh_bind_accept(bind, session)) != SSH_OK) {
    log_err("Error accepting connection: %s", ssh_get_error(bind));
    ssh_free(session);
    return -1;
  } else {
    log_debug("SSH connection accepted");
  }

  if (ssh_handle_key_exchange(session) != SSH_OK) {
    log_err("Error in key exchange: %s", ssh_get_error(session));
    ssh_free(session);
    return -1;
  } else {
    log_debug("Key exchange done");
  }

  if ((grs_session = session_create(grs)) == NULL) {
    log_err("Failed to create a GRS session");
    ssh_free(session);
    return -1;
  } else {
    log_debug("GRS session created");
  }

  if ((entry = session_list_prepend(slist, session)) == NULL) {
    log_err("Failed to queue session");
    ssh_free(session);
    return -1;
  } else {
    entry->grs_session = grs_session;
    log_debug("Session is queued");
  }

  // Handle initial data for the session
  handle_ssh_session(slist, entry, grs);

  return 0;
}

int main(int argc, char** argv) {
  struct sigaction sa;
  ssh_bind bind;
  char* hostkey = "";
  int port = 22;
  int writable = 0;
  int ch, result;
  grs_t grs;
  struct session_list session_list;

  memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = leave_handler;

  sigaction(SIGINT, &sa, NULL);

  while ((ch = getopt(argc, argv, "p:k:w?")) != -1) {
    switch (ch) {
    case 'p':
      port = atoi(optarg);
      break;
    case 'k':
      hostkey = optarg;
      break;
      case 'w':
        writable = 1;
        break;
    default:
      usage();
    }
  }

  grs = grs_init();

  grs_register_command(grs, "info", grs_info);

  SESSION_LIST_INIT(session_list);

  setup_acl(grs, writable);

  if ((bind = ssh_bind_new()) != NULL) {
    log_debug("SSH server bind created");
  } else {
    log_err("Failed to create the SSH server bind");
    return 1;
  }

  if ((result = ssh_bind_options_set(bind, SSH_BIND_OPTIONS_BINDPORT, &port)) == SSH_OK) {
    log_debug("SSH server listing on %i", port);
  } else {
    log_err("Failed to set SSH server port: %s", ssh_get_error(bind));
    ssh_bind_free(bind);
    return 1;
  }

  if ((result = ssh_bind_options_set(bind, SSH_BIND_OPTIONS_RSAKEY, hostkey)) == SSH_OK) {
    log_debug("SSH server hostkey: %s", hostkey);
  } else {
    log_err("Failed to set SSH server hostkey: %s", ssh_get_error(bind));
    ssh_bind_free(bind);
    return 1;
  }

  if (ssh_bind_listen(bind) == 0) {
    log_debug("SSH server is listining");
  } else {
    log_err("Error listening to socket: %s", ssh_get_error(bind));
    ssh_bind_free(bind);
    return 1;
  }

  while (1) {
    struct session_entry* entry;
    fd_set read_fds, write_fds;
    int max_fd;

    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_SET(ssh_bind_get_fd(bind), &read_fds);
    max_fd = ssh_bind_get_fd(bind);

    SESSION_LIST_FOREACH(entry, session_list) {
      buffer_t outbuf, errbuf;

      FD_SET(ssh_get_fd(entry->session), &read_fds);

      // Also register session-fd in write-set if you have buffered data to
      // write back to the client.
      outbuf = session_get_out_buffer(entry->grs_session);
      errbuf = session_get_err_buffer(entry->grs_session);
      if (session_can_exec(entry->grs_session) &&
          (buffer_get_size(outbuf) > 0 || buffer_get_size(errbuf) > 0)) {
        FD_SET(ssh_get_fd(entry->session), &write_fds);
      }

      if (ssh_get_fd(entry->session) > max_fd) {
        max_fd = ssh_get_fd(entry->session);
      }
    }

    log_debug("# of queued sessions: %i", session_list.size);

    result = select(max_fd + 1, &read_fds, &write_fds, NULL, NULL);

    if (result == -1 && errno == EINTR) {
      if (leave_selection_loop) {
        log_debug("Leaving selection loop");
        break;
      }

      continue;
    }

    if (FD_ISSET(ssh_bind_get_fd(bind), &read_fds)) {
      // Connection-attempt
      handle_ssh_bind(bind, &session_list, grs);
    } else {
      SESSION_LIST_FOREACH(entry, session_list) {
        if (FD_ISSET(ssh_get_fd(entry->session), &read_fds)) {
          if (!session_can_exec(entry->grs_session)) {
            // handle initial ssh-handshake
            handle_ssh_session(&session_list, entry, grs);
          }

          if (session_can_exec(entry->grs_session) &&
              ssh_channel_poll(entry->channel, 0) > 0) {
            // Data available in the channel, forward to process
            channel2buf(&session_list, entry);
          }
        }

        if (FD_ISSET(ssh_get_fd(entry->session), &write_fds)) {
          // Data available in the process, forward to the channel
          buf2channel(&session_list, entry, 0);
          buf2channel(&session_list, entry, 1);
        }

        // Execute the process
        if (session_can_exec(entry->grs_session) &&
            !session_is_finished(entry->grs_session)) {

          int result = session_exec(entry->grs_session);
          ssh_channel_request_send_exit_status(entry->channel,
                                               (result == 0) ? 0 : 127);
        }

        if (session_is_finished(entry->grs_session)) {
          buffer_t outbuf = session_get_out_buffer(entry->grs_session);
          buffer_t errbuf = session_get_err_buffer(entry->grs_session);
          if (buffer_get_size(outbuf) == 0 && buffer_get_size(errbuf) == 0) {
            // No cached data left, it's save to destroy the session now
            close_and_free_session_entry(&session_list, entry);
          }
        }
      }
    }
  }

  ssh_bind_free(bind);
  grs_destroy(grs);

  return 0;
}

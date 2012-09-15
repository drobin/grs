#include <sys/errno.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include <log.h>
#include <session.h>

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

static void close_and_free_session_entry(struct session_list* list,
                                         struct session_entry* entry) {
  if (entry->process != NULL) {
    process_destroy(entry->process);
  }

  if (entry->env != NULL) {
    process_env_destroy(entry->env);
  }

  if (entry->channel != NULL) {
    ssh_channel_close(entry->channel);
    ssh_channel_free(entry->channel);
  }

  ssh_free(entry->session);
  session2_destroy(entry->grs_session);
  session_list_remove(list, entry);
}

static int handle_ssh_authentication(session2_t session, ssh_message msg) {
  char* user;
  char* password;

  log_debug("Handle authentication for session");

  if (ssh_message_type(msg) != SSH_REQUEST_AUTH) {
    log_debug("Ignoring message of type %i", ssh_message_type(msg));

    ssh_message_reply_default(msg);

    return 0;
  }

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


  if (session2_authenticate(session, user, password) == 0) {
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

  if (ssh_message_type(msg) != SSH_REQUEST_CHANNEL_OPEN) {
    log_debug("Ignoring message of type %i", ssh_message_type(msg));

    ssh_message_reply_default(msg);

    return 0;
  }

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
                                      ssh_message msg) {
  log_debug("Handle channel-request");

  if (ssh_message_type(msg) != SSH_REQUEST_CHANNEL) {
    log_debug("Ignoring message of type %i", ssh_message_type(msg));

    ssh_message_reply_default(msg);

    return 0;
  }

  if (ssh_message_subtype(msg) != SSH_CHANNEL_REQUEST_EXEC) {
    log_debug("Ignoring channel-type %i", ssh_message_subtype(msg));

    ssh_message_reply_default(msg);

    return 0;
  }

  ssh_message_channel_request_reply_success(msg);
  log_debug("Channel request accepted");

  entry->env = process_env_create();
  entry->process = process_prepare(
    entry->env, ssh_message_channel_request_command(msg));

  session2_set_process(entry->grs_session, entry->process);
  session2_exec(entry->grs_session);

  return 0;
}

static int handle_ssh_session(struct session_list* list,
                              struct session_entry* entry) {
  ssh_message msg;

  log_debug("Session is selected");

  if ((msg = ssh_message_get(entry->session)) == NULL) {
    log_err("Failed to read message from session: %s",
            ssh_get_error(entry->session));

    close_and_free_session_entry(list, entry);

    return -1;
  }

  switch (session2_get_state(entry->grs_session)) {
    case NEED_AUTHENTICATION:
      log_debug("Session state: NEED_AUTHENTICATION");
      handle_ssh_authentication(entry->grs_session, msg);
      break;
    case NEED_PROCESS:
      log_debug("Session state: NEED_PROCESS");
      if (entry->channel == NULL) {
        handle_ssh_channel_open(entry, msg);
      } else {
        handle_ssh_channel_request(entry, msg);
      }
      break;
    default:
      log_debug("Session state: default");
      close_and_free_session_entry(list, entry);
      break;
  }

  ssh_message_free(msg);

  return 0;
}

static int handle_ssh_bind(ssh_bind bind, struct session_list* slist) {
  struct session_entry* entry;
  ssh_session session;
  session2_t grs_session;
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

  if ((grs_session = session2_create()) == NULL) {
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
  handle_ssh_session(slist, entry);

  return 0;
}

static int process2channel(struct session_list* list,
                           struct session_entry* entry) {
  char buf[512];
  size_t nread;
  size_t nwritten = 0;

  nread = read(process_get_fd_out(entry->process), buf, 512);

  if (nread == 0) {
    log_debug("EOF from fd_out");
    return -1;
  } else if (nread < 0) {
    log_err("Error occured while reading from process: %s", strerror(errno));
    return -1;
  }

  if (entry->channel == NULL) {
    log_err("You don't have a destination-channel");
    return -1;
  }

  log_debug("%i bytes read from process", nread);

  while (nwritten < nread) {
    int nbytes = ssh_channel_write(entry->channel,
                                   buf + nwritten, nread - nwritten);

    if (nbytes == SSH_ERROR) {
      log_err("Failed to write into channel: %s",
              ssh_get_error(entry->channel));
      return -1;
    }

    nwritten += nbytes;
    log_debug("%i bytes written into channel", nbytes);
  }

  return 0;
}

int main(int argc, char** argv) {
  struct sigaction sa;
  ssh_bind bind;
  char* hostkey = "";
  int port = 22;
  int ch, result;
  struct session_list session_list;

  memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = leave_handler;

  sigaction(SIGINT, &sa, NULL);

  while ((ch = getopt(argc, argv, "p:k:?")) != -1) {
    switch (ch) {
    case 'p':
      port = atoi(optarg);
      break;
    case 'k':
      hostkey = optarg;
      break;
    default:
      usage();
    }
  }

  SESSION_LIST_INIT(session_list);

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
    ssh_channel channels[session_list.size + 1];
    ssh_channel outchannels[session_list.size + 1];
    int nchannels = 0;
    int nprocesses = 0;
    struct session_entry* entry;
    fd_set read_fds;
    int max_fd;

    FD_ZERO(&read_fds);
    FD_SET(ssh_bind_get_fd(bind), &read_fds);
    max_fd = ssh_bind_get_fd(bind);

    SESSION_LIST_FOREACH(entry, session_list) {
      FD_SET(ssh_get_fd(entry->session), &read_fds);

      if (ssh_get_fd(entry->session) > max_fd) {
        max_fd = ssh_get_fd(entry->session);
      }

      if (entry->process != NULL) {
        nprocesses++;
        FD_SET(process_get_fd_out(entry->process), &read_fds);

        if (process_get_fd_out(entry->process) > max_fd) {
          max_fd = process_get_fd_out(entry->process);
        }
      }

      if (entry->channel != NULL) {
        channels[nchannels] = entry->channel;
        nchannels++;
      }
    }

    channels[nchannels] = NULL;

    log_debug("# of queued sessions: %i", session_list.size);
    log_debug("# of queued channels: %i", nchannels);
    log_debug("# of queued processes: %i", nprocesses);

    result = ssh_select(channels, outchannels, max_fd + 1, &read_fds, NULL);

    if (result == SSH_EINTR) {
      if (leave_selection_loop) {
        log_debug("Leaving selection loop");
        break;
      }

      continue;
    }

    if (FD_ISSET(ssh_bind_get_fd(bind), &read_fds)) {
      handle_ssh_bind(bind, &session_list);
    } else {
      SESSION_LIST_FOREACH(entry, session_list) {
        if (entry->process != NULL &&
            FD_ISSET(process_get_fd_out(entry->process), &read_fds)) {

          if (process2channel(&session_list, entry) != 0 ||
              process_get_status(entry->process, NULL) == 0) {
            close_and_free_session_entry(&session_list, entry);
            break;
          }
        }

        if (FD_ISSET(ssh_get_fd(entry->session), &read_fds)) {
          handle_ssh_session(&session_list, entry);
        }
      }
    }
  }

  ssh_bind_free(bind);

  return 0;
}

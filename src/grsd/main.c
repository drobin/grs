#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include <log.h>
#include <session.h>

#include "session_list.h"

static void leave_handler(int sig) {
  log_debug("Ask to leave grsd_listen");
}

static void usage() {
  printf("USAGE\n");
  exit(1);
}

int main(int argc, char** argv) {
  struct sigaction sa;
  ssh_bind bind;
  char* hostkey = "";
  int port = 22;
  int ch, result;
  struct session_head session_list;

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

  LIST_INIT(&session_list);

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
    ssh_channel channels[1], outchannels[1];
    struct session_entry* entry;
    fd_set read_fds;
    int max_fd;

    FD_ZERO(&read_fds);
    FD_SET(ssh_bind_get_fd(bind), &read_fds);
    max_fd = ssh_bind_get_fd(bind);

    LIST_FOREACH(entry, &session_list, entries) {
      FD_SET(ssh_get_fd(entry->session), &read_fds);

      if (ssh_get_fd(entry->session) > max_fd) {
        max_fd = ssh_get_fd(entry->session);
      }
    }

    channels[0] = NULL;

    result = ssh_select(channels, outchannels, max_fd + 1, &read_fds, NULL);

    if (result == SSH_EINTR) {
      continue;
    }

    if (FD_ISSET(ssh_bind_get_fd(bind), &read_fds)) {
      struct session_entry* entry;
      ssh_session session;

      log_debug("SSH server bind selected");

      session = ssh_new();

      if ((result = ssh_bind_accept(bind, session)) == SSH_OK) {
        log_debug("SSH connection accepted");
      } else {
        log_err("Error accepting connection: %s", ssh_get_error(bind));
      }

      if (ssh_handle_key_exchange(session) == SSH_OK) {
        log_debug("Key exchange done");
      } else {
        log_err("Error in key exchange: %s", ssh_get_error(session));
      }

      if ((entry = session_list_prepend(&session_list, session)) != NULL) {
        log_debug("Session is queued");
      } else {
        log_err("Failed to queue session");
      }
    } else {
      LIST_FOREACH(entry, &session_list, entries) {
        if (FD_ISSET(ssh_get_fd(entry->session), &read_fds)) {
          log_debug("Session is selected");

          ssh_free(entry->session);
          session_list_remove(entry);
        }
      }
    }
  }

  ssh_bind_free(bind);

  return 0;
}

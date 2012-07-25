#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "grsd.h"
#include "log.h"
#include "types.h"

static int stdout2channel(int fd, ssh_channel channel) {
  int nread;
  char buf[512];

  // Read data from stdout
  nread = read(fd, buf, sizeof(buf));

  if (nread > 0) {
    // Write data into channel
    log_debug("%i bytes read from stdout", nread);
    int nwritten = ssh_channel_write(channel, buf, nread);
    log_debug("%i bytes written into channel", nwritten);

    return 0;
  } else if (nread == 0) {
    log_debug("EOF from stdout");
    return -1;
  } else {
    log_err("Failed to read from stdout: %s", strerror(errno));
    return -1;
  }
}

static int channel2stdin(int fd, ssh_channel channel) {
  int nread;
  char buf[512];

  nread = ssh_channel_read(channel, buf, sizeof(buf), 0);
  if (nread > 0) {
    // write data into fd
    log_debug("%i bytes read from channel", nread);
    int nwritten = write(fd, buf, nread);
    log_debug("%i bytes written into stdin", nwritten);

    return 0;
  } else if (nread == 0) {
    log_debug("EOF from channel");
    return -1;
  } else {
    log_err("Failed to read from channel: %s", ssh_get_error(channel));
    return -1;
  }
}

static int session_exec(session_t session, ssh_message msg) {
  int pipe_in[2];
  int pipe_out[2];

  if (pipe(pipe_in) == -1) {
    log_err("Failed to create a pipe: %s", strerror(errno));
    return -1;
  }

  if (pipe(pipe_out) == -1) {
    log_err("Failed to create a pipe: %s", strerror(errno));
    return -1;
  }

  if ((session->pid = fork()) == -1) {
    log_err("Failed to fork: %s", strerror(errno));
    return -1;
  }

  if (session->pid == 0) { // The child executes the command
    char* cmd = ssh_message_channel_request_command(msg);

    log_debug("Executing '%s'", cmd);

    close(pipe_in[1]);
    close(pipe_out[0]);
    dup2(pipe_in[0], 0);
    dup2(pipe_out[1], 1);

    execl("/bin/ls", "-1", NULL);
    log_err("Failed to exec: %s", strerror(errno));
    _exit(1);
  } else {
    int result, stat_loc;

    close(pipe_in[0]);
    close(pipe_out[1]);

    while (1) {
      ssh_channel channels[2], outchannels[2];
      fd_set fds;
      int maxfd, result;

      channels[0] = session->channel;
      channels[1] = NULL;
      maxfd = ssh_get_fd(session->session);

      FD_ZERO(&fds);
      FD_SET(pipe_out[0], &fds);
      if (pipe_out[0] > maxfd) {
        maxfd = pipe_out[0];
      }

      result = ssh_select(channels, outchannels, maxfd + 1, &fds, NULL);
      if (result == EINTR) {
        continue;
      }

      if (FD_ISSET(pipe_out[0], &fds)) {
        if (stdout2channel(pipe_out[0], session->channel) == -1) {
          break;
        }
      }

      if (outchannels[0] != NULL) {
        if (channel2stdin(pipe_in[1], session->channel) == -1) {
          break;
        }
      }
    }

    close(pipe_in[1]);
    close(pipe_out[0]);

    if ((result = waitpid(session->pid, &stat_loc, 0)) == -1) {
      log_err("wait_pid: %s", strerror(errno));
    } else if (WIFEXITED(stat_loc)) {
      log_debug("Process terminated with %i", WEXITSTATUS(stat_loc));
    } else {
      log_err("Abnormal termination of process");
    }

    channel_send_eof(session->channel);
    session_destroy(session);
  }

  return 0;
}

static int session_handle_auth(session_t session, ssh_message msg) {
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

  if (strcmp(user, password)) {
    log_debug("Authentication rejected");
    ssh_message_reply_default(msg);
    return 0;
  }

  log_debug("Authentication succeeded");
  ssh_message_auth_reply_success(msg, 0);
  session->state = CHANNEL_OPEN;

  return 0;
}

static int session_handle_channel_open(session_t session, ssh_message msg) {
  log_debug("Handle open-request for a channel");

  if (ssh_message_type(msg) != SSH_REQUEST_CHANNEL_OPEN) {
    log_debug("Ignoring message of type %i", ssh_message_type(msg));

    ssh_message_reply_default(msg);

    return 0;
  }

  session->channel = ssh_message_channel_request_open_reply_accept(msg);
  log_debug("Channel is open");

  session->state = REQUEST_CHANNEL;

  return 0;
}

static int session_handle_request_channel(session_t session, ssh_message msg) {
  log_debug("Handle channel-request");

  if (ssh_message_type(msg) != SSH_REQUEST_CHANNEL) {
    log_debug("Ignoring message of type %i", ssh_message_type(msg));

    ssh_message_reply_default(msg);

    return 0;
  }

  if (ssh_message_subtype(msg) != SSH_CHANNEL_REQUEST_EXEC) {
    log_debug("Ignoring channel-type %i", ssh_message_subtype(msg));

    ssh_message_reply_default(msg);

    return -1;
  }

  ssh_message_channel_request_reply_success(msg);
  log_debug("Channel request accepted");

  session_exec(session, msg);

  session->state = NOP; // Finished

  return 0;
}

void grsd_handle_session(evutil_socket_t fd, short what, void* arg) {
  session_t session = (session_t)arg;
  ssh_message msg = ssh_message_get(session->session);
  int result;

  log_debug("Handle incoming data from session");

  if (ssh_message_type(msg) == -1) {
    log_debug("ssh_message_type of -1 received. Abort...");
    session_destroy(session);
    return;
  }

  // Depending on the session-state handle the ssh-data.
  // If the return-code is less than 0, the the handler asks to remove the
  // related event from the event_base_loop and destroy the session
  switch (session->state) {
    case AUTH:
      result = session_handle_auth(session, msg); break;
    case CHANNEL_OPEN:
      result = session_handle_channel_open(session, msg); break;
    case REQUEST_CHANNEL:
      result = session_handle_request_channel(session, msg); break;
    case NOP:
      result = -1; break;
  }


  ssh_message_free(msg);

  if (result < 0) {
    session_destroy(session);
  }
}

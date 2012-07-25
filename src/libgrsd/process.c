#include <sys/errno.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "process.h"

static void close_pipes(int pipe_in[2], int pipe_out[2]) {
  close(pipe_in[0]);
  close(pipe_in[1]);
  close(pipe_out[0]);
  close(pipe_out[1]);
}

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

int grs_process_prepare(struct grs_process* process, const char* command) {
  if (process == NULL || command == NULL) {
    return -1;
  }

  process->command = command;

  return 0;
}

int grs_process_exec(struct grs_process* process, ssh_channel channel) {
  int pipe_in[2];
  int pipe_out[2];
  pid_t pid;

  if (process == NULL || channel == NULL) {
    return -1;
  }

  if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
    log_err("Failed to create a pipe: %s", strerror(errno));
    close_pipes(pipe_in, pipe_out);
    return -1;
  }

  if ((pid = fork()) == -1) {
    log_err("Failed to fork: %s", strerror(errno));
    close_pipes(pipe_in, pipe_out);
    return -1;
  }

  if (pid == 0) { // The child executes the command
    log_debug("Executing '%s'", process->command);

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

      channels[0] = channel;
      channels[1] = NULL;
      maxfd = ssh_get_fd(ssh_channel_get_session(channel));

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
        if (stdout2channel(pipe_out[0], channel) == -1) {
          break;
        }
      }

      if (outchannels[0] != NULL) {
        if (channel2stdin(pipe_in[1], channel) == -1) {
          break;
        }
      }
    }

    close(pipe_in[1]);
    close(pipe_out[0]);

    if ((result = waitpid(pid, &stat_loc, 0)) == -1) {
      log_err("wait_pid: %s", strerror(errno));
    } else if (WIFEXITED(stat_loc)) {
      log_debug("Process terminated with %i", WEXITSTATUS(stat_loc));
    } else {
      log_err("Abnormal termination of process");
    }

    channel_send_eof(channel);

    return WEXITSTATUS(stat_loc);
  }
}

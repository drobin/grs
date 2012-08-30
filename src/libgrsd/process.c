#include <sys/errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "process.h"

static void close_pipes(int pipe_in[2], int pipe_out[2]) {
  close(pipe_in[0]);
  close(pipe_in[1]);
  close(pipe_out[0]);
  close(pipe_out[1]);
}

int grs_process_prepare(struct grs_process* process, const char* command) {
  if (process == NULL || command == NULL) {
    return -1;
  }

  process->command = command;

  return 0;
}

int grs_process_exec(struct grs_process* process, session_t session) {
  int pipe_in[2];
  int pipe_out[2];
  pid_t pid;

  if (process == NULL || session == NULL) {
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

    session_multiplex(session, pipe_out[0], pipe_in[1]);
    result = waitpid(pid, &stat_loc, 0);

    close(pipe_in[1]);
    close(pipe_out[0]);

    if (result == -1) {
      log_err("wait_pid: %s", strerror(errno));
    } else if (WIFEXITED(stat_loc)) {
      log_debug("Process terminated with %i", WEXITSTATUS(stat_loc));
    } else if (WIFSIGNALED(stat_loc)) {
      int signal = WTERMSIG(stat_loc);
      log_err("Process received the signal %i (%s)", signal, strsignal(signal));
    } else {
      log_err("Abnormal termination of process");
    }

    return WEXITSTATUS(stat_loc);
  }
}

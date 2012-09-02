#include <sys/errno.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "process.h"

struct _process {
};

struct _process_info {
  char* raw_token;
  char* token[ARG_MAX];
};

static void tokenize(struct _process_info* process_info) {
  char* token;
  int nargs = 0;

  for (; (token = strsep(&process_info->raw_token, " \t")) != NULL; nargs++) {
    process_info->token[nargs] = token;
  }

  process_info->token[nargs] = NULL;
}

static void close_pipes(int pipe_in[2], int pipe_out[2]) {
  close(pipe_in[0]);
  close(pipe_in[1]);
  close(pipe_out[0]);
  close(pipe_out[1]);
}

static int fork_exec(process_info_t process_info, session_t session) {
  int pipe_in[2];
  int pipe_out[2];
  pid_t pid;

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
    const char* cmd = process_info_get_command(process_info);

    log_debug("Executing '%s'", cmd);

    close(pipe_in[1]);
    close(pipe_out[0]);
    dup2(pipe_in[0], 0);
    dup2(pipe_out[1], 1);

    execvp(cmd, process_info->token);
    log_err("Failed to exec: %s", strerror(errno));
    _exit(127);
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

process_t process_init() {
  struct _process* process;

  if ((process = malloc(sizeof(struct _process))) == NULL) {
    return NULL;
  }

  return process;
}

int process_destroy(process_t process) {
  if (process == NULL) {
    return -1;
  }

  free(process);

  return 0;
}

process_info_t process_prepare(process_t process, const char* command) {
  struct _process_info* process_info;

  if (process == NULL || command == NULL) {
    return NULL;
  }

  if ((process_info = malloc(sizeof(struct _process_info))) == NULL) {
    return NULL;
  }

  process_info->raw_token = strdup(command);
  tokenize(process_info);

  return process_info;
}

int process_info_destroy(process_info_t process_info) {
  if (process_info == NULL) {
    return -1;
  }

  free(process_info->raw_token);
  free(process_info);

  return 0;
}

const char* process_info_get_command(process_info_t process_info) {
  if (process_info == NULL) {
    return NULL;
  }

  return process_info->token[0];
}

const char** process_info_get_args(process_info_t process_info) {
  if (process_info == NULL) {
    return NULL;
  }

  return (const char**)process_info->token + 1;
}

int process_exec(process_t process, process_info_t process_info,
                 session_t session) {
  if (process == NULL || process_info == NULL || session == NULL) {
    return -1;
  }

  if (strcmp(process_info->token[0], "git-upload-pack") == 0 ||
      strcmp(process_info->token[0], "git-receive-pack") == 0) {
    char* repository = process_info->token[1];

    if (repository[0] == '\'' && repository[strlen(repository) - 1] == '\'') {
      repository[strlen(repository) - 1] = '\0';
      process_info->token[1] = repository + 1;
    }
  }

  return fork_exec(process_info, session);
}

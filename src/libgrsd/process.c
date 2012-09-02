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

struct _process_env {
};

struct _process {
  char* raw_token;
  char* token[ARG_MAX];
};

static void tokenize(struct _process* process) {
  char* token;
  int nargs = 0;

  for (; (token = strsep(&process->raw_token, " \t")) != NULL; nargs++) {
    process->token[nargs] = token;
  }

  process->token[nargs] = NULL;
}

static void close_pipes(int pipe_in[2], int pipe_out[2]) {
  close(pipe_in[0]);
  close(pipe_in[1]);
  close(pipe_out[0]);
  close(pipe_out[1]);
}

static int fork_exec(process_t process, session_t session) {
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
    const char* cmd = process_info_get_command(process);

    log_debug("Executing '%s'", cmd);

    close(pipe_in[1]);
    close(pipe_out[0]);
    dup2(pipe_in[0], 0);
    dup2(pipe_out[1], 1);

    execvp(cmd, process->token);
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

process_env_t process_env_create() {
  struct _process_env* env;

  if ((env = malloc(sizeof(struct _process_env))) == NULL) {
    return NULL;
  }

  return env;
}

int process_env_destroy(process_env_t env) {
  if (env == NULL) {
    return -1;
  }

  free(env);

  return 0;
}

process_t process_prepare(process_env_t env, const char* command) {
  struct _process* process;

  if (env == NULL || command == NULL) {
    return NULL;
  }

  if ((process = malloc(sizeof(struct _process))) == NULL) {
    return NULL;
  }

  process->raw_token = strdup(command);
  tokenize(process);

  return process;
}

int process_destroy(process_t process) {
  if (process == NULL) {
    return -1;
  }

  free(process->raw_token);
  free(process);

  return 0;
}

const char* process_info_get_command(process_t process) {
  if (process == NULL) {
    return NULL;
  }

  return process->token[0];
}

const char** process_info_get_args(process_t process) {
  if (process == NULL) {
    return NULL;
  }

  return (const char**)process->token + 1;
}

int process_exec(process_env_t env, process_t process, session_t session) {
  if (env == NULL || process == NULL || session == NULL) {
    return -1;
  }

  if (strcmp(process->token[0], "git-upload-pack") == 0 ||
      strcmp(process->token[0], "git-receive-pack") == 0) {
    char* repository = process->token[1];

    if (repository[0] == '\'' && repository[strlen(repository) - 1] == '\'') {
      repository[strlen(repository) - 1] = '\0';
      process->token[1] = repository + 1;
    }
  }

  return fork_exec(process, session);
}

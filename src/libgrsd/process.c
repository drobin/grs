#include <sys/errno.h>
#include <sys/queue.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "process.h"

struct command_entry {
  char* command;
  command_hook hook;
  LIST_ENTRY(command_entry) entries;
};

LIST_HEAD(command_head, command_entry);

struct _process_env {
  struct command_head cmd_head;
};

struct process_info {
  pid_t pid;
  int exited;
  int exit_status;
};

struct _process {
  struct _process_env* env;
  char* raw_token;
  char* token[ARG_MAX];
  int in_fds[2];
  int out_fds[2];
  struct process_info info;
};

static void tokenize(struct _process* process) {
  char* token;
  int nargs = 0;

  for (; (token = strsep(&process->raw_token, " \t")) != NULL; nargs++) {
    process->token[nargs] = token;
  }

  process->token[nargs] = NULL;
}

static int fork_exec(process_t process) {
  pid_t pid;

  if ((pid = fork()) == -1) {
    log_err("Failed to fork: %s", strerror(errno));
    return -1;
  }

  if (pid == 0) { // The child executes the command
    const char* cmd = process_get_command(process);

    log_debug("Executing '%s'", cmd);

    close(process->in_fds[1]);
    close(process->out_fds[0]);
    dup2(process->in_fds[0], 0);
    dup2(process->out_fds[1], 1);

    execvp(cmd, process->token);
    log_err("Failed to exec: %s", strerror(errno));
    _exit(127);
  } else {
    process->info.pid = pid;
    return 0;
  }
}

process_env_t process_env_create() {
  struct _process_env* env;

  if ((env = malloc(sizeof(struct _process_env))) == NULL) {
    return NULL;
  }

  LIST_INIT(&env->cmd_head);

  return env;
}

int process_env_destroy(process_env_t env) {
  struct command_entry* entry;

  if (env == NULL) {
    return -1;
  }

  while ((entry = LIST_FIRST(&(env->cmd_head))) != NULL) {
    LIST_REMOVE(entry, entries);
    free(entry->command);
    free(entry);
  }

  free(env);

  return 0;
}

int process_env_register_command(process_env_t env, const char* command,
                                 command_hook hook) {
  struct command_entry* entry;

  if (env == NULL || command == NULL || hook == NULL) {
    return -1;
  }

  if ((entry = malloc(sizeof(struct command_entry))) == NULL) {
    return -1;
  }

  entry->command = strdup(command);
  entry->hook = hook;
  LIST_INSERT_HEAD(&(env->cmd_head), entry, entries);

  return 0;
}

command_hook process_env_get_command(process_env_t env, const char* command) {
  struct command_entry* entry;
  if (env == NULL || command == NULL) {
    return NULL;
  }

  LIST_FOREACH(entry, &(env->cmd_head), entries) {
    if (strcmp(entry->command, command) == 0) {
      return entry->hook;
    }
  }

  return NULL;
}

process_t process_prepare(process_env_t env, const char* command) {
  struct _process* process;

  if (env == NULL || command == NULL) {
    return NULL;
  }

  if ((process = malloc(sizeof(struct _process))) == NULL) {
    return NULL;
  }

  memset(process, 0, sizeof(struct _process));

  process->env = env;
  process->raw_token = strdup(command);
  tokenize(process);

  if (pipe(process->in_fds) != 0) {
    log_err("Failed to create in_fds: %s", strerror(errno));
    process_destroy(process);
    return NULL;
  }

  if (pipe(process->out_fds) != 0) {
    log_err("Failed to create out_fds: %s", strerror(errno));
    process_destroy(process);
    return NULL;
  }

  return process;
}

int process_destroy(process_t process) {
  if (process == NULL) {
    return -1;
  }

  free(process->raw_token);
  close(process->in_fds[0]);
  close(process->in_fds[1]);
  close(process->out_fds[0]);
  close(process->out_fds[1]);
  free(process);

  return 0;
}

process_env_t process_get_env(process_t process) {
  if (process == NULL) {
    return NULL;
  }

  return process->env;
}

const char* process_get_command(process_t process) {
  if (process == NULL) {
    return NULL;
  }

  return process->token[0];
}

const char** process_get_args(process_t process) {
  if (process == NULL) {
    return NULL;
  }

  return (const char**)process->token + 1;
}

int process_get_fd_in(process_t process) {
  if (process == NULL) {
    return -1;
  }

  return process->in_fds[1];
}

int process_get_fd_out(process_t process) {
  if (process == NULL) {
    return -1;
  }

  return process->out_fds[0];
}

int process_exec(process_t process) {
  if (process == NULL) {
    return -1;
  }

  if (strcmp(process->token[0], "test") == 0) {
    write(process->out_fds[1], "Hello world!", 12);
    close(process->out_fds[1]);
    return 0;
  }

  if (strcmp(process->token[0], "git-upload-pack") == 0 ||
      strcmp(process->token[0], "git-receive-pack") == 0) {
    char* repository = process->token[1];

    if (repository[0] == '\'' && repository[strlen(repository) - 1] == '\'') {
      repository[strlen(repository) - 1] = '\0';
      process->token[1] = repository + 1;
    }
  }

  return fork_exec(process);
}

int process_update_status(process_t process) {
  if (process == NULL) {
    return -1;
  }

  if (process->info.pid == 0) {
    // Non-forked process, already finished
    process->info.exited = 1;
    process->info.exit_status = 0;
    close(process->out_fds[1]);
    return 0;
  } else {
    // This is a forked process, check with waitpid
    int result, stat_loc;

    if (process->info.exited) {
      // Already exited, nothing to check here
      return 0;
    }

    result = waitpid(process->info.pid, &stat_loc, WNOHANG);

    if (result == 0) {
      // Still running
      return 0;
    } else if (result < 0) {
      log_err("wait_pid: %s", strerror(errno));
      return -1;
    } else {
      process->info.exited = 1;
      close(process->out_fds[1]);

      if (WIFEXITED(stat_loc)) {
        process->info.exit_status = WEXITSTATUS(stat_loc);
        log_debug("Process terminated with %i", WEXITSTATUS(stat_loc));
      } else if (WIFSIGNALED(stat_loc)) {
        int signal = WTERMSIG(stat_loc);
        log_err("Process received the signal %i (%s)",
                signal, strsignal(signal));
      } else {
        log_err("Abnormal termination of process");
      }
    }

    close(process->out_fds[1]);

    return 0;
  }
}

int process_is_exited(process_t process) {
  if (process == NULL) {
    return -1;
  }

  if (process_update_status(process) != 0) {
    return -1;
  }

  return process->info.exited;
}

int process_get_exit_status(process_t process) {
  if (process == NULL) {
    return -1;
  }

  if (process_update_status(process) != 0) {
    return -1;
  }

  if (process->info.exited) {
    return process->info.exit_status;
  } else {
    return -1;
  }
}

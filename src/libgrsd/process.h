#ifndef PROCESS_H
#define PROCESS_H

#include <libssh/libssh.h>

struct grs_process {
  const char *command;
};

int grs_process_prepare(struct grs_process* process, const char* command);
int grs_process_exec(struct grs_process* process, ssh_channel channel);

#endif  /* PROCESS_H */

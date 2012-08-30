#ifndef PROCESS_H
#define PROCESS_H

#include "session.h"

struct grs_process {
  const char *command;
};

int grs_process_prepare(struct grs_process* process, const char* command);
int grs_process_exec(struct grs_process* process, session_t session);

#endif  /* PROCESS_H */

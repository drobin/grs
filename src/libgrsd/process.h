#ifndef PROCESS_H
#define PROCESS_H

struct grs_process {
  const char *command;
};

int grs_process_prepare(struct grs_process* process, const char* command);

#endif  /* PROCESS_H */

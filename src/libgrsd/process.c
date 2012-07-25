#include <stdlib.h>

#include "process.h"

int grs_process_prepare(struct grs_process* process, const char* command) {
  if (process == NULL || command == NULL) {
    return -1;
  }

  process->command = command;

  return 0;
}

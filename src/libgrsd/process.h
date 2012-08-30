#ifndef PROCESS_H
#define PROCESS_H

#include "session.h"

struct _process;
typedef struct _process* process_t;

process_t grs_process_init(const char* command);
int grs_process_destroy(process_t process);
const char* grs_process_get_command(process_t process);
const char** grs_process_get_args(process_t process);
int grs_process_exec(process_t process, session_t session);

#endif  /* PROCESS_H */

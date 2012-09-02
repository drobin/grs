#ifndef PROCESS_H
#define PROCESS_H

#include "session.h"

struct _process_env;
struct _process;

typedef struct _process_env* process_env_t;
typedef struct _process* process_t;

/**
 * Initializes the process module.
 *
 * A <code>process_t</code> structure represents the environment where
 * processes can be executed.
 *
 * @return A new process environment
 */
process_env_t process_env_create();

/**
 * Destroys the process environment again.
 *
 * @param env The process environment to be destroyed.
 * @return On success <code>0</code> is returned.
 */
int process_env_destroy(process_env_t env);

/**
 * Prepares the execution of the given <code>command</code> in the environment
 * of <code>process</code>.
 *
 * @param env The process environment
 * @param command The command (incl. arguments) to be executed.
 * @return On success a process_t-structure is returned, which can be
 *         executed by process_exec().
 */
process_t process_prepare(process_env_t env, const char* command);

/**
 * Destroys <code>process</code> again.
 *
 * @param process The process to be destroyed
 * @return On success <code>0</code> is returned.
 */
int process_destroy(process_t process);

/**
 * Returns the environment of the process.
 *
 * @param process The process to scan
 * @return The environment of the process
 */
process_env_t process_get_env(process_t process);

/**
 * Returns the command to be executed of the given <code>process</code>.
 *
 * Usually this is the path of the binary or something similar.
 *
 * @param process The process to scan
 * @return The command to be executed
 */
const char* process_get_command(process_t process);

/**
 * Returns the arguments passed to the command of the given
 * <code>process</code>.
 *
 * @param process The process to scan
 * @return The arguments passed to the command
 */
const char** process_get_args(process_t process);

/**
 * Executes a process in the given <code>session</code>.
 *
 * @param process Information about the process to be executed
 * @param session The target session, where the process is executed
 * @return The exit-code of the process. Usually an exit-code of <code>0</code>
 *         means success.
 */
int process_exec(process_t process, session_t session);

#endif  /* PROCESS_H */

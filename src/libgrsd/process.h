#ifndef PROCESS_H
#define PROCESS_H

#include "session.h"

struct _process_env;
struct _process_info;

typedef struct _process_env* process_env_t;
typedef struct _process_info* process_info_t;

/**
 * Initializes the process module.
 *
 * A <code>process_t</code> structure represents the environment where
 * processes can be executed.
 *
 * @return A new process environment
 */
process_env_t process_init();

/**
 * Destroys the process environment again.
 *
 * @param env The process environment to be destroyed.
 * @return On success <code>0</code> is returned.
 */
int process_destroy(process_env_t env);

/**
 * Prepares the execution of the given <code>command</code> in the environment
 * of <code>process</code>.
 *
 * @param env The process environment
 * @param command The command (incl. arguments) to be executed.
 * @return On success a process_info_t-structure is returned, which can be
 *         executed by process_exec().
 */
process_info_t process_prepare(process_env_t env, const char* command);

/**
 * Destroys <code>process_info</code> again.
 *
 * @param process_info The process information to be destroyed
 * @return On success <code>0</code> is returned.
 */
int process_info_destroy(process_info_t process_info);

/**
 * Returns the command to be executed of the given <code>process_info</code>.
 *
 * Usually this is the path of the binary or something similar.
 *
 * @param process_info The process_info to scan
 * @return The command to be executed
 */
const char* process_info_get_command(process_info_t process_info);

/**
 * Returns the arguments passed to the command of the given
 * <code>process_info</code>.
 *
 * @param process_info The process_info to scan
 * @return The arguments passed to the command
 */
const char** process_info_get_args(process_info_t process_info);

/**
 * Executes a process in the given <code>session</code>.
 *
 * @param env The process environment
 * @param process_info Information about the process to be executed
 * @param session The target session, where the process is executed
 * @return The exit-code of the process. Usually an exit-code of <code>0</code>
 *         means success.
 */
int process_exec(process_env_t env, process_info_t process_info,
                 session_t session);

#endif  /* PROCESS_H */

#ifndef PROCESS_H
#define PROCESS_H

struct _process_env;
struct _process;

typedef struct _process_env* process_env_t;
typedef struct _process* process_t;
typedef int (*command_hook)(process_t);

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
 * Register a command-hook at the process-environment.
 *
 * If <code>command</code> should be executed, then the given <code>hook</code>
 * is invoked.
 *
 * @param env The destination environment
 * @param command The command, where the hook should be registered
 * @param hook The hook which gets invoked
 * @return On success <code>0</code> is returned.
 */
int process_env_register_command(process_env_t env, const char* command,
                                 command_hook hook);

/**
 * Receives the hook of an already registered command.
 *
 * @param env The destination environment
 * @param command The requested command
 * @return The hook registered for the given command. If no such command is
 *         registered, <code>NULL</code> is returned.
 */
command_hook process_env_get_command(process_env_t env, const char* command);

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
 * Returns the filedescriptor to give data to the process.
 *
 * You you want to give some data to the process, then use this filedescriptor
 * write into it.
 *
 * @param process The process
 * @return The filedescriptor you need write into it to give some data to the
 *         process.
 */
int process_get_fd_in(process_t process);

/**
 * Returns the filedescriptor the process uses to give back some data.
 *
 * You need to <i>read</i> data from the filedescriptor.
 *
 * @param process The process
 * @return The filedescriptor to read to receive data from the process
 */
int process_get_fd_out(process_t process);

/**
 * Executes a process in the given <code>session</code>.
 *
 * The process runs until reading from process_get_fd_out() return EOF.
 *
 * @param process Information about the process to be executed
 * @return The exit-code of the process. Usually an exit-code of <code>0</code>
 *         means success.
 */
int process_exec(process_t process);

/**
 * Updates the status-information about a (possible) running process.
 *
 * Call this function before you want to read from process_get_fd_out().
 * Otherwise you might never read an EOF from the file-descriptor.
 *
 * @param process The process
 * @return On success <code>0</code> is returned. It tells you nothing about
 *         the status of the process. For this you need to call
 *         process_get_status().
 */
int process_update_status(process_t process);

/**
 * Checks if the running process has already exited.
 *
 * @param process The process to ask
 * @return If the process has already exited, then true is returned.
 */
int process_is_exited(process_t process);

/**
 * If the process has already exited, then the function returns the exit-status
 * of the process.
 *
 * @param process The process to ask
 * @return The exit-status of the process
 */
int process_get_exit_status(process_t process);

#endif  /* PROCESS_H */

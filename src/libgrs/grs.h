/*******************************************************************************
 *
 * This file is part of grs.
 *
 * grs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * grs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with grs.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#ifndef GRSD_H
#define GRSD_H

#include "acl.h"
#include "buffer.h"

struct _grs;

/**
 * Handle to the grs-daemon-process.
 */
typedef struct _grs* grs_t;

typedef int (*command_hook)(const char** command,
                            buffer_t in_buf, buffer_t out_buf,
                            buffer_t err_buf);

/*
 * Creates a new grs-handle.
 *
 * @return A new grs-handle or <code>NULL</code> on error.
 */
grs_t grs_init();

/**
 * Destroys the grs-handle again.
 *
 * @param handle The handle to be destroyed.
 * @return On success <code>0</code> is returned, on error <code>-1</code>.
 */
int grs_destroy(grs_t handle);

/**
 * Returns the ACL-system assigned to rge grs-handle.
 *
 * @param handle The handle
 * @return The ACL-system assigned to this grs-handle
 */
acl_t grs_get_acl(grs_t handle);

/**
 * Register a command-hook at the grs-handle.
 *
 * If <code>command</code> should be executed, then the given <code>hook</code>
 * is invoked.
 *
 * @param handle The handle
 * @param command The command, where the hook should be registered. The array
 *                containts the command and all its arguments. The array must
 *                be NULL-terminared and you need at least one array-element.
 * @param hook The hook which gets invoked
 * @return On success <code>0</code> is returned.
 */
int grs_register_command(grs_t handle, char *const command[],
                         command_hook hook);

/**
 * Receives the hook of an already registered command.
 *
 * First it tries to get the hook for the command and all its arguments. If
 * you don't have a hook at this position, then the function tries to find the
 * hook by removing the last argument from the search-path. The algorithm is
 * repeated until the search-path is empty.
 *
 * @param handle The handle
 * @param command Array which contains the command and all its arguments.The
 *                array must be NULL-terminated and you need at least one
 *                array-element.
 * @return The hook registered for the given command. If no such command is
 *         registered, <code>NULL</code> is returned.
 */
command_hook grs_get_command(grs_t handle, char *const command[]);

#endif /* GRSD_H */

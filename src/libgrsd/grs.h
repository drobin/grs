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

typedef int (*command2_hook)(const char** command,
                            buffer_t in_buf, buffer_t out_buf);

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
 * @param command The command, where the hook should be registered
 * @param hook The hook which gets invoked
 * @return On success <code>0</code> is returned.
 */
int grs_register_command(grs_t handle, const char* command, command2_hook hook);

/**
 * Receives the hook of an already registered command.
 *
 * @param handle The handle
 * @param command The requested command
 * @return The hook registered for the given command. If no such command is
 *         registered, <code>NULL</code> is returned.
 */
command2_hook grs_get_command(grs_t handle, const char* command);

#endif /* GRSD_H */

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

#include "process.h"

struct _grs;

/**
 * Handle to the grs-daemon-process.
 */
typedef struct _grs* grs_t;

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
 * Returns the process-environment assigned to the grs-handle.
 *
 * @param handle The handle
 * @return The process-environment of this grs-handle
 */
process_env_t grs_get_process_env(grs_t handle);

#endif /* GRSD_H */

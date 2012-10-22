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

#include <stdlib.h>

#include <libgrs/buffer.h>

#include "core.h"

static int grs_info(char *const command[], buffer_t in_buf, buffer_t out_buf,
                    buffer_t err_buf) {
  buffer_append(out_buf, "** INFO **\n", 11);
  return 0;
}

int load_core_extension(grs_t grs) {
  char* command[] = { "grs", "info", NULL };
  struct command_hooks hooks;

  hooks.init = NULL;
  hooks.exec = grs_info;

  return grs_register_command(grs, command, &hooks);
}

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

#include "git.h"
#include "protocol.h"

int rd_get_refs_stub(struct rd_ref** refs, size_t* nrefs) {
  *nrefs = 0;
  return 0;
}

static int git_upload_pack(char *const command[], buffer_t in_buf,
                           buffer_t out_buf, buffer_t err_buf) {
  return reference_discovery(out_buf, err_buf, rd_get_refs_stub);
}

int load_git_extension(grs_t grs) {
  char* command[] = { "git-upload-pack", NULL };
  return grs_register_command(grs, command, git_upload_pack);
}

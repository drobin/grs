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

#include <git2.h>
#include <string.h>

#include <libgrs/log.h>

#include "git.h"
#include "protocol.h"

static char* repository_path(const char* str) {
  char* repository;

  // Strip leading "'" from the path
  if (str[0] == '\'') {
    repository = strdup(str + 1);
  } else {
    repository = strdup(str);
  }

  // Strip tailing "'" from the path
  if (repository[strlen(repository) - 1] == '\'') {
    repository[strlen(repository) - 1] = '\0';
  }

  return repository;
}

struct git_reference_foreach_data {
  git_repository* repo;
  struct rd_ref** refs;
  size_t* nrefs;
};

static int git_reference_foreach_cb(const char* ref_name, void* payload) {
  struct git_reference_foreach_data* data;
  git_reference* ref;
  char hex[41];
  int result;
  int nrefs;

  data = (struct git_reference_foreach_data*)payload;
  nrefs = *(data->nrefs);

  if ((result = git_reference_lookup(&ref, data->repo, ref_name)) != 0) {
    log_err("Reference lookup failed: %s", git_strerror(result));
    return 0; // Skip reference but try another one
  }

  if (nrefs == 0) {
    *(data->refs) = malloc(sizeof(struct rd_ref));
  } else {
    *(data->refs) = realloc(*(data->refs), sizeof(struct rd_ref) * (nrefs + 1));
  }

  git_oid_fmt(hex, git_reference_oid(ref));
  hex[40] = '\0';

  (*(data->refs))[nrefs].obj_id = strdup(hex);
  (*(data->refs))[nrefs].ref_name = strdup(git_reference_name(ref));
  *(data->nrefs) += 1;

  return 0;
}

static int get_refs_impl(const char* repository,
                        struct rd_ref** refs, size_t* nrefs) {

  struct git_reference_foreach_data data;
  git_repository* repo;
  int result;

  log_debug("Fetch reference from %s", repository);

  if ((result = git_repository_open(&repo, repository)) == 0) {
    log_debug("Repository is open");
  } else {
    log_err("Failed to open repository: %s", git_strerror(result));
    return result;
  }

  *nrefs = 0;
  data.repo = repo;
  data.refs = refs;
  data.nrefs = nrefs;
  result = git_reference_foreach(repo, GIT_REF_LISTALL,
                                 git_reference_foreach_cb, &data);
  if (result != 0) {
    log_err("Failed to loop over references: %s", git_strerror(result));
    git_repository_free(repo);
    return result;
  }

  git_repository_free(repo);

  return 0;
}

static int git_upload_pack(char *const command[], buffer_t in_buf,
                           buffer_t out_buf, buffer_t err_buf) {

  char *repository;
  int result;

  repository = repository_path(command[1]);
  result = reference_discovery(repository, out_buf, err_buf, get_refs_impl);

  free(repository);

  return result;
}

int load_git_extension(grs_t grs) {
  char* command[] = { "git-upload-pack", NULL };
  return grs_register_command(grs, command, git_upload_pack);
}

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

/**
 * List of processes executed by git-upload-pack
 */
enum upload_pack_process {
  /**
   * reference_discovery()
   */
  p_reference_discovery = 0,

  /**
   * packfile_negotiation()
   */
  p_packfile_negotiation,

  /**
   * No more processes!
   */
  p_finished
};

/**
 * Payload-data for the git-upload-pack-command.
 */
struct git_upload_pack_data {
  /**
   * Path of the repository
   */
  char* repository;

  /**
   * Data used internally by packfile_negotiation().
   */
  struct packfile_negotiation_data packfile_negotiation;

  /**
   * Currently executed process
   */
  enum upload_pack_process current_process;
};

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
  binbuf_t refs;
};

static int git_reference_foreach_cb(const char* ref_name, void* payload) {
  struct git_reference_foreach_data* data;
  git_reference* ref;
  int result;
  struct git_ref* gref;

  data = (struct git_reference_foreach_data*)payload;

  if ((result = git_reference_lookup(&ref, data->repo, ref_name)) != 0) {
    log_err("Reference lookup failed: %s", git_strerror(result));
    return 0; // Skip reference but try another one
  }

  gref = binbuf_add(data->refs);
  git_oid_fmt(gref->obj_id, git_reference_oid(ref));
  gref->obj_id[40] = '\0';
  strlcpy(gref->ref_name, git_reference_name(ref), sizeof(gref->ref_name));

  return 0;
}

static int get_refs_impl(const char* repository, binbuf_t refs) {
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

  data.repo = repo;
  data.refs = refs;
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

static int init_git_upload_pack(char *const command[], void** payload) {
  struct git_upload_pack_data* data;

  data = malloc(sizeof(struct git_upload_pack_data));
  data->repository = repository_path(command[1]);
  memset(&data->packfile_negotiation, 0,
         sizeof(struct packfile_negotiation_data));
  data->current_process = p_reference_discovery;
  *payload = data;

  return 0;
}

static int git_upload_pack(buffer_t in_buf, buffer_t out_buf,
                           buffer_t err_buf, void* payload) {

  struct git_upload_pack_data* data = (struct git_upload_pack_data*)payload;
  int result;

  if (buffer_get_size(out_buf) > 0 || buffer_get_size(err_buf) > 0) {
    // You still have pending write-data, wait until everything is written back
    // to the client
    return 1;
  }

  switch (data->current_process) {
  case p_reference_discovery:
    log_debug("reference discovery on %s", data->repository);
    result = reference_discovery(data->repository, out_buf, err_buf,
                                 get_refs_impl);
    break;
  case p_packfile_negotiation:
    log_debug("packfile negotiation on %s", data->repository);
    result = packfile_negotiation(in_buf, out_buf, &data->packfile_negotiation);
    break;
  default:
    log_err("Unsupported process requested: %i", data->current_process);
    result = -1;
    break;
  }

  if (result == 0) { // Success
    // Switch to the next process
    data->current_process++;

    if (data->current_process < p_finished) {
      // (Sub-process) finished, but there's at least another pending process.
      result = 1;
    }
  }

  return result;
}

static void destroy_git_upload_pack(void* payload) {
  struct git_upload_pack_data* data = (struct git_upload_pack_data*)payload;

  free(data->repository);
  free(data);
}

int load_git_extension(grs_t grs) {
  char* command[] = { "git-upload-pack", NULL };
  struct command_hooks hooks;

  hooks.init = init_git_upload_pack;
  hooks.exec = git_upload_pack;
  hooks.destroy = destroy_git_upload_pack;

  return grs_register_command(grs, command, &hooks);
}

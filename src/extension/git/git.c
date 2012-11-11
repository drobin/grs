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

#include <string.h>

#include <libgrs/log.h>

#include "git.h"
#include "libgit2.h"
#include "protocol.h"

/**
 * List of processes executed by git-upload-pack
 */
enum upload_pack_process {
  /**
   * reference_discovery()
   */
  p_upload_pack_reference_discovery = 0,

  /**
   * packfile_negotiation()
   */
  p_upload_pack_packfile_negotiation,

  /**
   * packfile_transfer()
   */
  p_upload_pack_packfile_transfer,

  /**
   * No more processes!
   */
  p_upload_pack_finished
};

/**
 * List of processes executed by git-receive-pack
 */
enum receive_pack_process {
  /**
   * reference_discovery()
   */
  p_receive_pack_reference_discovery = 0,

  /**
   * update_request()
   */
  p_receive_pack_update_request,

  /**
   * report_status()
   */
  p_receive_pack_report_status,

  /**
   * No more processes!
   */
  p_receive_pack_finished
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
   * The results from the packfile-negotiation.
   */
  struct packfile_negotiation_result result;

  /**
   * Data used internally by packfile_negotiation().
   */
  struct packfile_negotiation_data packfile_negotiation;

  /**
   * Currently executed process
   */
  enum upload_pack_process current_process;
};

/**
 * Payload-data for the git-receive-pack-command.
 */
struct git_receive_pack_data {
  /**
   * Path of the repository.
   */
  char* repository;

  /**
   * Currently executed process
   */
  enum receive_pack_process current_process;
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

static int init_git_upload_pack(char *const command[], void** payload) {
  struct git_upload_pack_data* data;

  data = malloc(sizeof(struct git_upload_pack_data));
  data->repository = repository_path(command[1]);
  data->result.capabilities = 0;
  data->result.commits = binbuf_create(41);
  memset(&data->packfile_negotiation, 0,
         sizeof(struct packfile_negotiation_data));
  data->current_process = p_upload_pack_reference_discovery;
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
  case p_upload_pack_reference_discovery:
    log_debug("reference discovery on %s", data->repository);
    result = reference_discovery(data->repository, process_upload_pack, out_buf,
                                 err_buf, libgit2_reference_discovery_cb);
    break;
  case p_upload_pack_packfile_negotiation:
    log_debug("packfile negotiation on %s", data->repository);
    result = packfile_negotiation(data->repository, in_buf, out_buf,
                                  &data->result, libgit2_commit_log_cb,
                                  &data->packfile_negotiation);
    break;
  case p_upload_pack_packfile_transfer:
    log_debug("packfile transfer on %s", data->repository);
    result = packfile_transfer(data->repository, data->result.commits,
                               libgit2_packfile_objects_cb, out_buf);
    break;
  default:
    log_err("Unsupported process requested: %i", data->current_process);
    result = -1;
    break;
  }

  if (result == 0) { // Success
    // Switch to the next process
    data->current_process++;

    if (data->current_process < p_upload_pack_finished) {
      // (Sub-process) finished, but there's at least another pending process.
      result = 1;
    }
  } else if (result == 2) { // Success, but don't execute another sub-process
    data->current_process = p_upload_pack_finished;
    result = 0;
  }

  return result;
}

static void destroy_git_upload_pack(void* payload) {
  struct git_upload_pack_data* data = (struct git_upload_pack_data*)payload;

  free(data->repository);
  binbuf_destroy(data->result.commits);
  free(data);
}

static int init_git_receive_pack(char *const command[], void** payload) {
  struct git_receive_pack_data* data;

  data = malloc(sizeof(struct git_receive_pack_data));
  data->repository = repository_path(command[1]);
  data->current_process = p_receive_pack_reference_discovery;

  *payload = data;

  return 0;
}

static int git_receive_pack(buffer_t in_buf, buffer_t out_buf,
                            buffer_t err_buf, void* payload) {
  struct git_receive_pack_data* data = (struct git_receive_pack_data*)payload;
  int result;

  if (buffer_get_size(out_buf) > 0 || buffer_get_size(err_buf) > 0) {
    // You still have pending write-data, wait until everything is written back
    // to the client
    return 1;
  }

  do {
    switch (data->current_process) {
    case p_receive_pack_reference_discovery:
      log_debug("reference discovery on %s", data->repository);
      result = reference_discovery(data->repository, process_receive_pack,
                                   out_buf, err_buf,
                                   libgit2_reference_discovery_cb);
      break;
    case p_receive_pack_update_request:
      log_debug("update request on %s", data->repository);
      result = update_request(data->repository, in_buf);
      break;
    case p_receive_pack_report_status:
      log_debug("report status on %s", data->repository);
      result = report_status(data->repository);
      break;
    default:
      log_err("Unsupported process requested: %i", data->current_process);
      result = -1;
      break;
    }

    if (result == 0 || result == 3) {
      // Sucess, switch to next sub-process
      data->current_process++;
    }
  } while (result == 3); // result of 3 means, that the next-process should
                         // be executed immediately. Don't wait for new
                         // input-data.

  if (result == 0) { // Success
    if (data->current_process < p_receive_pack_finished) {
      // (Sub-process) finished, but there's at least another pending process.
      result = 1;
    }
  } else if (result == 2) { // Success, but don't execute another sub-process
    data->current_process = p_receive_pack_finished;
    result = 0;
  }

  return result;
}

static void destroy_git_receive_pack(void* payload) {
  struct git_receive_pack_data* data = (struct git_receive_pack_data*)payload;

  free(data->repository);
  free(data);
}

int load_git_extension(grs_t grs) {
  char* upload_pack_command[] = { "git-upload-pack", NULL };
  char* receive_pack_command[] =  { "git-receive-pack", NULL };
  struct command_hooks upload_pack_hooks;
  struct command_hooks receive_pack_hooks;
  int result;

  upload_pack_hooks.init = init_git_upload_pack;
  upload_pack_hooks.exec = git_upload_pack;
  upload_pack_hooks.destroy = destroy_git_upload_pack;

  receive_pack_hooks.init = init_git_receive_pack;
  receive_pack_hooks.exec = git_receive_pack;
  receive_pack_hooks.destroy = destroy_git_receive_pack;

  result = grs_register_command(grs, upload_pack_command, &upload_pack_hooks) -
    grs_register_command(grs, receive_pack_command, &receive_pack_hooks);
  return result < 0 ? -1 : result;
}

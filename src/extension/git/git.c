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
   * packfile_transfer()
   */
  p_packfile_transfer,

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
   * Commits (as a result of the packfile-negotiation), which are send to the
   * client.
   */
  binbuf_t commits;

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

static int reference_is_branch_or_tag(const char* ref_name) {
  const char* branch_prefix = "refs/heads";
  const char* tag_prefix = "refs/tags";

  return strncmp(ref_name, branch_prefix, strlen(branch_prefix)) == 0 ||
         strncmp(ref_name, tag_prefix, strlen(tag_prefix)) == 0;
}

static int git_reference_foreach_cb(const char* ref_name, void* payload) {
  struct git_reference_foreach_data* data;
  git_reference* ref;
  int result;
  struct git_ref* gref;

  if (!reference_is_branch_or_tag(ref_name)) {
    log_info("Skipping reference %s", ref_name);
    return 0;
  }

  data = (struct git_reference_foreach_data*)payload;

  if ((result = git_reference_lookup(&ref, data->repo, ref_name)) != 0) {
    log_err("Reference lookup failed: %s", giterr_last()->message);
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
  git_reference* head;
  int result;

  log_debug("Fetch reference from %s", repository);

  if ((result = git_repository_open(&repo, repository)) == 0) {
    log_debug("Repository is open");
  } else {
    log_err("Failed to open repository: %s", giterr_last()->message);
    return result;
  }

  // Fetch current HEAD from repository
  if ((result = git_repository_head(&head, repo)) == 0) {
    const git_oid* head_oid = git_reference_oid(head);
    struct git_ref* head_ref = binbuf_add(refs);

    git_oid_fmt(head_ref->obj_id, head_oid);
    head_ref->obj_id[40] = '\0';
    strlcpy(head_ref->ref_name, "HEAD", sizeof(head_ref->ref_name));
  } else {
    log_err("Failed to receive HEAD from %s: %s",
            repository, giterr_last()->message);
    git_repository_free(repo);
    return result;
  }

  // Fetch references from repository
  data.repo = repo;
  data.refs = refs;
  result = git_reference_foreach(repo, GIT_REF_LISTALL,
                                 git_reference_foreach_cb, &data);
  if (result != 0) {
    log_err("Failed to loop over references: %s", giterr_last()->message);
    git_repository_free(repo);
    return result;
  }

  git_repository_free(repo);

  return 0;
}

static void log_oid_err(const char* message, const git_oid* oid,
                        const char* err) {
  char hex[41];

  git_oid_fmt(hex, oid);
  hex[40] = '\0';

  log_err(message, hex, err);
}

static int create_and_add_packfile_object(const git_oid *obj_id, git_odb* odb,
                                          binbuf_t objects) {
  struct packfile_object* pf_obj;
  git_odb_object *obj;

  if (git_odb_read(&obj, odb, obj_id) != 0) {
    log_oid_err("Failed to read object %s from object database: %s",
                obj_id, giterr_last()->message);
    return -1;
  }

  pf_obj = binbuf_add(objects);
  pf_obj->type = git_odb_object_type(obj);
  pf_obj->content = buffer_create();
  buffer_append(pf_obj->content, git_odb_object_data(obj),
                git_odb_object_size(obj));

  git_odb_object_free(obj);

  log_debug("packfile-object: %s", git_object_type2string(pf_obj->type));

  return 0;
}

struct treewalk_cb_data {
  git_odb* odb;
  binbuf_t objects;
};

static int treewalk_cb(const char* root, const git_tree_entry* entry,
                       void* payload) {
  struct treewalk_cb_data* data = (struct treewalk_cb_data*)payload;
  const git_oid* oid = git_tree_entry_id(entry);

  return create_and_add_packfile_object(oid, data->odb, data->objects);
}


static int packfile_objects_for_commit(git_odb* odb, git_commit* commit,
                                       binbuf_t objects) {
  git_tree* tree;
  struct treewalk_cb_data data;

  // The commit-object
  if (create_and_add_packfile_object(git_commit_id(commit), odb, objects) != 0) {
    return -1;
  }

  // The tree-object
  if (git_commit_tree(&tree, commit) != 0) {
    log_oid_err("Failed to receive tree object %s from object database: %s",
                git_commit_id(commit), giterr_last()->message);
    return -1;
  }

  if (create_and_add_packfile_object(git_tree_id(tree), odb, objects) != 0) {
    git_tree_free(tree);
    return -1;
  }

  // The tree-entries
  data.odb = odb;
  data.objects = objects;
  if (git_tree_walk(tree, treewalk_cb, GIT_TREEWALK_PRE, &data) != 0) {
    log_oid_err("Failed to read commit-tree for %s from object database: %s",
                git_commit_id(commit), giterr_last()->message);
    git_tree_free(tree);
    return -1;
  }

  git_tree_free(tree);

  return 0;
}

static int commit_log_impl(const char* repository, const char* obj_id,
                           binbuf_t commits) {
  git_repository* repo;
  git_revwalk* walk;
  git_oid oid;
  int result;

  log_debug("Fetch commit-log from %s", repository);

  if ((result = git_repository_open(&repo, repository)) == 0) {
    log_debug("Repository is open");
  } else {
    log_err("Failed to open repository: %s", giterr_last()->message);
  }

  git_oid_fromstr(&oid, obj_id);

  if (git_revwalk_new(&walk, repo) != 0) {
    log_err("Failed to creata revision-walker: %s", giterr_last()->message);

    git_repository_free(repo);

    return -1;
  }

  git_revwalk_sorting(walk, GIT_SORT_TOPOLOGICAL | GIT_SORT_REVERSE);

  if (git_revwalk_push(walk, &oid) != 0) {
    log_oid_err("Failed to push %s on revision-walker: %s",
                &oid, giterr_last()->message);

    git_revwalk_free(walk);
    git_repository_free(repo);

    return -1;
  }

  while ((git_revwalk_next(&oid, walk)) == 0) {
    char* hex = binbuf_add(commits);
    git_oid_fmt(hex, &oid);
    hex[40] = '\0';
    log_debug("log %s", hex);
  }

  git_revwalk_free(walk);
  git_repository_free(repo);

  return 0;
}

static int packfile_objects_impl(const char* repository, binbuf_t commits,
                                 binbuf_t objects) {
  git_repository* repo;
  git_odb* odb;
  int result, idx;

  repo = NULL;
  odb = NULL;

  log_debug("Fetch commit objects from %s", repository);

  if ((result = git_repository_open(&repo, repository)) == 0) {
    log_debug("Repository is open");
  } else {
    log_err("Failed to open repository: %s", giterr_last()->message);
  }

  if (result == 0) {
    if ((result = git_repository_odb(&odb, repo)) == 0) {
      log_debug("Object database is open");
    } else {
      log_err("Failed to open object database: %s", giterr_last()->message);
    }
  }

  for (idx = 0; idx < binbuf_get_size(commits) && result == 0; idx++) {
    const char* hex = binbuf_get(commits, idx);
    git_commit* commit;
    git_oid oid;

    git_oid_fromstr(&oid, hex);
    if ((result = git_commit_lookup(&commit, repo, &oid)) == 0) {
      log_debug("Extracting objects for commit %s", hex);
      result = packfile_objects_for_commit(odb, commit, objects);
      git_commit_free(commit);
    } else {
      log_err("Commit lookup for %s failed: %s", hex, giterr_last()->message);
    }
  }

  if (odb != NULL) {
    git_odb_free(odb);
  }

  if (repo != NULL) {
    git_repository_free(repo);
  }

  return result;
}

static int init_git_upload_pack(char *const command[], void** payload) {
  struct git_upload_pack_data* data;

  data = malloc(sizeof(struct git_upload_pack_data));
  data->repository = repository_path(command[1]);
  data->commits = binbuf_create(41);
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
    result = packfile_negotiation(data->repository, in_buf, out_buf,
                                  data->commits, commit_log_impl,
                                  &data->packfile_negotiation);
    break;
  case p_packfile_transfer:
    log_debug("packfile transfer on %s", data->repository);
    result = packfile_transfer(data->repository, data->commits,
                               packfile_objects_impl, out_buf);
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
  } else if (result == 2) { // Success, but don't execute another sub-process
    data->current_process = p_finished;
    result = 0;
  }

  return result;
}

static void destroy_git_upload_pack(void* payload) {
  struct git_upload_pack_data* data = (struct git_upload_pack_data*)payload;

  free(data->repository);
  binbuf_destroy(data->commits);
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

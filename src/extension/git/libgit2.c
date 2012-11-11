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

#include "libgit2.h"
#include "protocol.h"

struct git_reference_foreach_data {
  git_repository* repo;
  binbuf_t refs;
};

struct treewalk_cb_data {
  git_odb* odb;
  binbuf_t objects;
};

static void log_oid_err(const char* message, const git_oid* oid,
                        const char* err) {
  char hex[41];

  git_oid_fmt(hex, oid);
  hex[40] = '\0';

  log_err(message, hex, err);
}

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

int libgit2_reference_discovery_cb(const char* repository, binbuf_t refs) {
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
    // TODO It is better to distinguish between
    //      (1) no HEAD available
    //      (2) any other error (which is a real error in this case)
    log_warn("Failed to receive HEAD from %s: %s",
             repository, giterr_last()->message);
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

int libgit2_commit_log_cb(const char* repository, const char* obj_id,
                          const binbuf_t haves, binbuf_t commits,
                          int* common_base) {
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

  git_revwalk_sorting(walk, GIT_SORT_TOPOLOGICAL);

  if (git_revwalk_push(walk, &oid) != 0) {
    log_oid_err("Failed to push %s on revision-walker: %s",
                &oid, giterr_last()->message);

    git_revwalk_free(walk);
    git_repository_free(repo);

    return -1;
  }

  while ((git_revwalk_next(&oid, walk)) == 0) {
    char hex[41];
    int idx;

    git_oid_fmt(hex, &oid);
    hex[40] = '\0';

    if ((idx = binbuf_find(haves, hex, 40)) >= 0) {
      *common_base = idx;
      log_debug("log (common-base): %s", hex);
      break;
    } else {
      char* commit_id = binbuf_add(commits);
      strlcpy(commit_id, hex, sizeof(hex));
      log_debug("log %s", hex);
    }
  }

  git_revwalk_free(walk);
  git_repository_free(repo);

  return 0;
}

int libgit2_packfile_objects_cb(const char* repository, binbuf_t commits,
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

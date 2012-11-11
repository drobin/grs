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

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <limits.h>

#include <libgrs/binbuf.h>
#include <libgrs/buffer.h>

/**
 * A reference.
 */
struct git_ref {
  /**
   * The SHA object-id of the reference
   */
  char obj_id[41];

  /**
   * The name of the reference
   */
  char ref_name[PATH_MAX];
};

/**
 * Phase of packfile-negotiation.
 */
enum packfile_negotiation_phase {
  /**
   * Preparation-phase
   */
  packfile_negotiation_prepare = 0,

  /**
   * upload-request
   */
  packfile_negotiation_upload_request,

  /**
   * upload-haves
   */
  packfile_negotiation_upload_haves,

  /**
   * finished, clean up
   */
  packfile_negotiation_finished,

  /**
   * Finished, cleanup, not execute another phase
   */
  packfile_negotiation_quit,

  /**
   * Finished with error, clean up
   */
  packfile_negotiation_error
};

/**
 * Data used to synchronize different invocations of packfile_negotiation().
 */
struct packfile_negotiation_data {
  /**
   * Current phase of negotiation
   */
  enum packfile_negotiation_phase phase;

  /**
   * A buffered pkt-line
   */
  buffer_t pkt_line;

  /**
   * want-list received from the client
   */
  binbuf_t want_list;

  /**
   * shallow-list received from the client
   */
  binbuf_t shallow_list;

  /**
   * have-list received from the client
   */
  binbuf_t have_list;

  /**
   * Maximum commit history depth received from the client
   */
  int depth;
};

/**
 * The result of the packfile_negotiation()-invocation.
 */
struct packfile_negotiation_result {
  /**
   * List of or'ed client-capabilities. The capabilities are defined in
   * <code>enum capabilities</code>.
   */
  int capabilities;

  /**
   * Buffer contains the commits as a result of the packfile-negotiation with
   * the client.
   */
  binbuf_t commits;
};

/**
 * A single object, which is encoded into a packfile.
 *
 * A packfile can contain more than one object.
 */
struct packfile_object {
  /**
   * Type of object.
   */
  unsigned char type;

  /**
   * The content of the object.
   */
  buffer_t content;
};

/**
 * A callback-function used by reference_discovery to fetch references from a
 * repository.
 *
 * The callback-implementation should store the references at <code>refs</code>.
 * The number of references should be stored in <code>nrefs</code>.
 *
 * @param repository The path the the requested repository
 * @param refs The implementation should store the references here.
 * @param nrefs The implementation should store the number of references here.
 * @return The implementation should return <code>0</code>, if the operation was
 *         successful.
 */
typedef int (*reference_discovery_cb)(const char* repository, binbuf_t refs);

/**
 * A callback-function used to collect a commit-log of the given object.
 *
 * This is similar to a <code>git log</code>-invocation. The implementation
 * should fill the <code>commits/code>-array wil the object-ids of the commits
 * of the log. <code>obj_id</code> should be also included.
 *
 * @param repository The path of the requested repository
 * @param obj_if The head of the log-operation
 * @param haves An array with object-ids the client already has. You don't need
 *              to transfer these commits.
 * @param commits The implementation should fill this array with the
 *                log-results.
 * @param common_base The implementation should fill this argument with the
 *                    index, where the base-commit in <code>haves</code> is
 *                    located. <code>commit_base</code> is predefined with
 *                    <code>-1</code>. So if you don't have a
 *                    commit-base-commit, fill it with <code>-1</code> or leave
 *                    it as it is.
 * @return On success <code>0</code> should be returned.
 */
typedef int (*commit_log_cb)(const char* repository, const char* obj_id,
                             const binbuf_t haves, binbuf_t commits,
                             int* common_base);

/**
 * A callback-functions used by packfile_transfer to fetch the objects for a
 * packfile.
 *
 * The callback-implementation should store the objects of type
 * <code>struct packfile_object</code> into the <code>objects</code>-argument.
 *
 * @param repository The path of the requested repository
 * @param commits An array with object-ids of COMMIT-objects to fetch from
 *                the repository.
 * @param objects The callback-implementation should store the objects here.
 */
typedef int (*packfile_objects_cb)(const char* repository, binbuf_t commits,
                                   binbuf_t objects);

/**
 * Implementation of the <i>Reference Discovery</i>-process.
 *
 * @param repository The path of the requested repository
 * @param out The function writes data into this buffer, which should be
 *            transferred to the client.
 * @param err The function writes error-messages into the buffer (if any).
 * @param refs A function used to fetch references from a repository.
 * @return On success <code>0</code> is returned.
 *
 * @see https://github.com/git/git/blob/master/Documentation/technical/pack-protocol.txt
 */
int reference_discovery(const char* repository,
                        buffer_t out, buffer_t err,
                        reference_discovery_cb refs);

/**
 * Implementation of the <i>Packfile Negotiation</i>-process.
 *
 * @param repository The path of the requested repository
 * @param in The function consumes data from this buffer
 * @param out The function writes data into this buffer, which should be send
 *            back to the client
 * @param pn_results The results of the packfile-negotiation. The function will
 *                   puts all the results into this structure.
 * @param data Data used to synchronize between different invocations of the
 *             function. Before the first invocation of packfile_negotiation()
 *             the structure should be set to <code>0</code>.
 * @return On success <code>0</code> is returned.
 * @see https://github.com/git/git/blob/master/Documentation/technical/pack-protocol.txt
 */
int packfile_negotiation(const char* repository, buffer_t in, buffer_t out,
                         struct packfile_negotiation_result* pn_results,
                         commit_log_cb log_cb,
                         struct packfile_negotiation_data* data);

/**
 * Stores the packfiles of the given commits into the given output-buffer.
 *
 * This is the last step of the <i>git-upload-pack</i>-process.
 *
 * @param repository The path of the requested repository
 * @param commits An array of hex-commits, which should be encoded into
 *                packfiles
 * @param obj_cb Callback is invoked by packfile-transfer to receive the objects
 *               for the packfile(s)
 * @param out Buffer, where to store the encoded packfiles
 * @return On success <code>0</code> is returned
 *
 * @see https://github.com/git/git/blob/master/Documentation/technical/pack-protocol.txt
 */
int packfile_transfer(const char* repository, binbuf_t commits,
                      packfile_objects_cb obj_cb, buffer_t out);

/**
 * Implementation of the <i>Update request</i> process.
 *
 * @param repository The pack of the requested repository
 * @param in The function consumes data from this buffer
 * @return On success <code>0</code> is returned.
 */
int update_request(const char* repository, buffer_t in);

/**
 * Implementation of the <i>Report status</i> process.
 *
 * @param repository The pack of the requested repository
 * @return On success <code>0</code> is returned.
 */
int report_status(const char* repository);

#endif  /* PROTOCOL_H */

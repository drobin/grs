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
typedef int (*rd_get_refs)(const char* repository, binbuf_t refs);

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
                        buffer_t out, buffer_t err, rd_get_refs refs);

/**
 * Implementation of the <i>Packfile Negotiation</i>-process.
 *
 * @param in The function consumes data from this buffer
 * @param out The function writes data into this buffer, which should be send
 *            back to the client
 * @param data Data used to synchronize between different invocations of the
 *             function. Before the first invocation of packfile_negotiation()
 *             the structure should be set to <code>0</code>.
 * @return On success <code>0</code> is returned.
 * @see https://github.com/git/git/blob/master/Documentation/technical/pack-protocol.txt
 */
int packfile_negotiation(buffer_t in, buffer_t out,
                         struct packfile_negotiation_data* data);

#endif  /* PROTOCOL_H */

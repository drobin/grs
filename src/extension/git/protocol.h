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

#include <libgrs/buffer.h>

/**
 * A pkg-line
 * @see https://github.com/git/git/blob/master/Documentation/technical/protocol-common.txt
 */
struct pkt_line {
  /**
   * Length of payload.
   *
   * If <code>len</code> is <code>0</code>, then this is a <i>flush-pkt</i>.
   */
  unsigned int len;

  /**
   * Payload encoded in the pkt-line.
   */
  char payload[0];
};

/**
 * Creates a new <i>pkt-line</i>.
 *
 * @param len Length of payload, which should be assigned to the
 *            <i>pkt-line</i>. If <code>len</code> is <code>0</code>, then
 *            this is a <i>flush-pkt</i>.
 * @param payload The payload to be assigned to the <i>pkt-line</i>. It can be
 *                <code>NULL</i>, when <code>len</code> is <code>0</code>.
 * @return A new <i>pkt-line</i>.
 */
struct pkt_line* pkt_line_create(int len, char* payload);

/**
 * Destroys the given <i>pkt-line</i> again.
 *
 * @param line The <i>pkt-line</i> to destroy.
 * @return On success <code>0</code> is returned.
 */
int pkt_line_destroy(struct pkt_line* line);

/**
 * Reads a <i>pkt-line</i> from the given buffer.
 *
 * If a <i>pkt-line</code> was read, the related data are removed from the
 * buffer.
 *
 * @param buf The source where to read the data.
 * @return A <i>pkt-line</i> read from the buffer. If not a complete
 *         <i>pkt-line<i> is available or the format of the <i>pkt-line</i> is
 *         wrong, <code>NULL</code> is returned.
 */
struct pkt_line* pkt_line_read(buffer_t buf);

/**
 * Writes a <i>pkt-line</i> into the buffer.
 *
 * @param line The <i>pkt-line</i> to encode into the buffer
 * @param buf The destination buffer
 * @return On success <code>0</code> is returned.
 */
int pkt_line_write(struct pkt_line* line, buffer_t buf);

#endif  /* PROTOCOL_H */

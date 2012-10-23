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

#ifndef PKT_LINE_H
#define PKT_LINE_H

#include <libgrs/buffer.h>

/**
 * Reads a <i>pkt-line</i> from the given buffer.
 *
 * Reads a <i>pkt_line</i> from <code>src</code>, decodes the data and writes
 * the result into <code>dest</code>. If a <i>pkt-line</code> was read, the
 * related data are removed from the buffer.
 *
 * @param src The source where to read the data.
 * @param dest The destination buffer, where to write the decoded data
 * @return On success <code>0</code> is returned. If there is no complete
 *         <i>pkt-line</i> is available in the buffer, then <code>1</code> is
 *         returned. If an error occured, <code>-1</code> is returned.
 */
int pkt_line_read(buffer_t src, buffer_t dest);

/**
 * Writes a <i>pkt-line</i> into the buffer.
 *
 * Assume, that the whole buffer-content of <code>src</code> should be encoded.
 * The resulting pkt_line is written into <code>dest</code>.
 *
 * @param src The source buffer. The content of the resulting pkt_line.
 * @param dest The destination buffer
 * @return On success <code>0</code> is returned.
 */
int pkt_line_write(buffer_t src, buffer_t dest);

#endif  /* PKT_LINE_H */

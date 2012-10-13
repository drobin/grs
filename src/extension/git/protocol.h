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
 * Implementation of the <i>Reference Discovery</i>-process.
 *
 * @param out The function writes data into this buffer, which should be
 *            transferred to the client.
 * @param err The function writes error-messages into the buffer (if any).
 * @return On success <code>0</code> is returned.
 *
 * @see https://github.com/git/git/blob/master/Documentation/technical/pack-protocol.txt
 */
int reference_discovery(buffer_t out, buffer_t err);

#endif  /* PROTOCOL_H */

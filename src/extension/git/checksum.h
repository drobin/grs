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

#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <libgrs/buffer.h>

/**
 * Calculates the SHA1-checksum over the content of the given buffer.
 *
 * The checksum is stored into the <code>sha</code>-argument.
 *
 * @param buffer The source-buffer
 * @param sha A 20-byte-wide array. The function stores the checksum here
 * @return On success <code>0</code> is returned.
 */
int buffer_checksum(buffer_t buffer, unsigned char sha[]);

#endif  /* CHECKSUM_H */

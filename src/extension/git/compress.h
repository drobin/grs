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

#ifndef COMPRESS_H
#define COMPRESS_H

#include <libgrs/buffer.h>

/**
 * Compresses the content of the <code>source</code>-buffer.
 *
 * The compressed result into written into the <code>dest</code>-buffer.
 *
 * @param source The source buffer, which contains the uncompressed data
 * @param dest The destination buffer. The function writes the compressed data
 *             into this buffer.
 * @return On success <code>0</code> is returned
 */
int buffer_compress(buffer_t source, buffer_t dest);

#endif  /* COMPRESS_H */

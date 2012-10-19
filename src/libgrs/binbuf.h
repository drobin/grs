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

#ifndef BINBUF_H
#define BINBUF_H

typedef struct _binbuf* binbuf_t;

/**
 * Create a new binbuf-structure.
 *
 * @param size The size of one element to be stored in the buffer.
 * @return The handle to the binbuf.
 */
binbuf_t binbuf_create(size_t size);

/**
 * Destroys the binbuf again.
 *
 * You will loose any items which are still stored in the buffer.
 *
 * @param buf The buffer to destroy
 * @return On success <code>0</code> is returned.
 */
int binbuf_destroy(binbuf_t buf);

#endif  /* BINBUF_H */

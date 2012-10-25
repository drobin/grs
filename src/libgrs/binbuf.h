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

#include <stdlib.h>

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

/**
 * Returns the number of bytes reserved for each item in the list.
 *
 * @param buf The requested buffer
 * @return The size of each item
 */
int binbuf_get_size_of(binbuf_t buf);

/**
 * Returns the size of the buffer.
 *
 * This is the number of items actually stored in the buffer.
 *
 * @param The requested buffer
 * @preturn The number of items stored in the buffer.
 */
int binbuf_get_size(binbuf_t buf);

/**
 * Returns the capacity of the buffer.
 *
 * This is the amount of items you can store in the buffer without reorganizing
 * it.
 *
 * @param buffer The requested buffer
 * @return The capacity of the buffer.
 */
int binbuf_get_capacity(binbuf_t buf);

/**
 * Returns the item at the given index.
 *
 * @param buf The requested buf
 * @param idx The index of the item to return: <code>0 <= idx < size</code>.
 * @return The item at the requested index. If <code>idx</code> is out of range,
 *         <code>NULL</code> is returned.
 */
void* binbuf_get(binbuf_t buf, unsigned int idx);

/**
 * Assigns a new item to the buffer.
 *
 * If the capacity of the buffer is not big enough to hold a new item, then
 * the buffer is increased to the desired size.
 *
 * @param buf The requested buffer
 * @param A pointer to the new item, which can be modified
 */
void* binbuf_add(binbuf_t buf);

#endif  /* BINBUF_H */

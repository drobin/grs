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

#ifndef BUFFER_H
#define BUFFER_H

struct _buffer;
typedef struct _buffer* buffer_t;

/**
 * Creates a new, empty buffer.
 *
 * @return The new buffer
 */
buffer_t buffer_create();

/**
 * Destroys the buffer again.
 *
 * Any data, which are still buffered, are lost!
 *
 * @param buffer The buffer to destroy
 * @return On success <code>0</code> is returned.
 */
int buffer_destroy(buffer_t buffer);

/**
 * Returns the size of the buffer.
 *
 * This is the number of bytes actually stored in the buffer.
 *
 * @param The requested buffer
 * @preturn The number of bytes stored in the buffer.
 */
int buffer_get_size(buffer_t buffer);

#endif  /* BUFFER_H */

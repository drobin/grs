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

#include <stdlib.h>
#include <zlib.h>

#include "compress.h"

int buffer_compress(buffer_t source, buffer_t dest) {
  z_stream strm;
  unsigned char out[128];
  int result;

  if (source == NULL || dest == NULL) {
    return -1;
  }

  // allocate deflate state
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;

  if ((result = deflateInit(&strm, Z_DEFAULT_COMPRESSION)) != Z_OK) {
    return -1;
  }

  strm.avail_in = buffer_get_size(source);
  strm.next_in = (void*)buffer_get_data(source);

  do {
    strm.avail_out = sizeof(out);
    strm.next_out = out;

    if ((result = deflate(&strm, Z_FINISH)) == Z_STREAM_ERROR) {
      deflateEnd(&strm);
      return -1;
    }

    buffer_append(dest, out, sizeof(out) - strm.avail_out);
  } while (strm.avail_out == 0);

  deflateEnd(&strm);

  return (result == Z_STREAM_END) ? 0 : -1;
}

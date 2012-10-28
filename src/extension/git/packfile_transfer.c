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

#include <arpa/inet.h>

#include <libgrs/log.h>

#include "checksum.h"
#include "compress.h"
#include "protocol.h"

static void objectsbuf_destroy(binbuf_t objects) {
  int idx;

  for (idx = 0; idx < binbuf_get_size(objects); idx++) {
    const struct packfile_object* obj = binbuf_get(objects, idx);
    buffer_destroy(obj->content);
  }

  binbuf_destroy(objects);
}

static int write_packfile_header(int nobjects, buffer_t out) {
  const uint32_t num = htonl(nobjects);
  unsigned char hdr[] = {
    'P', 'A', 'C', 'K',
    0x00, 0x00, 0x00, 0x02
  };

  return buffer_append(out, hdr, sizeof(hdr)) +
         buffer_append(out, &num, sizeof(num));
}

static int write_packfile_object_header(const struct packfile_object* object,
                                        buffer_t out) {
  uint8_t hdr_byte;
  size_t size = buffer_get_size(object->content);

  hdr_byte = (object->type & 0x7) << 4;
  hdr_byte |= size & 0xf;
  size >>= 4;

  while (size > 0) {
    hdr_byte |= 0x80;
    buffer_append(out, &hdr_byte, 1);

    hdr_byte = size & 0x7f;
    size >>= 7;
  }

  buffer_append(out, &hdr_byte, 1);

  return 0;
}

static int encode_packfile_object(const struct packfile_object* object,
                                  buffer_t out) {
  // Each packfile-object starts with a header, followed by the compressed
  // object-content
  return write_packfile_object_header(object, out) +
         buffer_compress(object->content, out);
}

int packfile_transfer(const char* repository, binbuf_t commits,
                      packfile_objects_cb obj_cb, buffer_t out) {
  binbuf_t objects;
  unsigned char sha[20];
  int idx;

  if (repository == NULL || commits == NULL || obj_cb == NULL || out == NULL) {
    return -1;
  }

  // Fetch packfile-objects from callback
  objects = binbuf_create(sizeof(struct packfile_object));
  if (obj_cb(repository, commits, objects) != 0) {
    objectsbuf_destroy(objects);
    return -1;
  }

  // Write global packfile-header
  if (write_packfile_header(binbuf_get_size(objects), out) != 0) {
    objectsbuf_destroy(objects);
    return -1;
  }

  // Encode packfile-objects and assign to output-buffer
  for (idx = 0; idx < binbuf_get_size(objects); idx++) {
    const struct packfile_object* object = binbuf_get(objects, idx);
    if (encode_packfile_object(object, out) != 0) {
      objectsbuf_destroy(objects);
      return -1;
    }
  }

  // Finally append the SHA1-checksum to the output-buffer
  if (buffer_checksum(out, sha) == 0) {
    buffer_append(out, sha, sizeof(sha));
  } else {
    objectsbuf_destroy(objects);
    return -1;
  }

  objectsbuf_destroy(objects);

  return 0;
}

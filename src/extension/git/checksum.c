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
#include <openssl/sha.h>

#include "checksum.h"

int buffer_checksum(buffer_t buffer, unsigned char sha[]) {
  if (buffer == NULL || sha == NULL) {
    return -1;
  }

  SHA1((unsigned char*)buffer_get_data(buffer), buffer_get_size(buffer), sha);

  return 0;
}

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
#ifndef CAPABILITIES_H
#define CAPABILITIES_H

#include <stddef.h>

/**
 * Enumeration of possible capabilities.
 *
 * @see https://github.com/git/git/blob/master/Documentation/technical/protocol-capabilities.txt
 */
enum capabilities {
  multi_ack = 0x1,
  multi_ack_detailed = 0x2,
  thin_pack = 0x4,
  side_band = 0x8,
  side_band_64k = 0x10,
  ofs_delta = 0x20,
  shallow = 0x40,
  no_progress = 0x80,
  include_tag = 0x100,
  report_status = 0x200,
  delete_refs = 0x400
};

/**
 * Parses the given string and extracts the encoded capabilities.
 *
 * @param str The string to parse
 * @param len The length of <code>str</code>.
 * @return List of or'ed capabilities. The capabilities are defined in
 *         <code>enum capabilities</code>. If an unsupported capability-string
 *         was detected, <code>-1</code> is returned.
 */
int capabilities_parse(const char* str, size_t len);

#endif  /* CAPABILITIES_H */

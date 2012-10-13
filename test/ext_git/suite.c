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

#include <check.h>

extern TCase* git_extension_tcase();
extern TCase* git_protocol_tcase();
extern TCase* pkt_line_tcase();

Suite* git_extension_suite() {
  Suite* s = suite_create("git extension");

  suite_add_tcase(s, git_extension_tcase());
  suite_add_tcase(s, git_protocol_tcase());
  suite_add_tcase(s, pkt_line_tcase());

  return s;
}

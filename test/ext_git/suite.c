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

extern TCase* capabilities_tcase();
extern TCase* checksum_tcase();
extern TCase* compress_tcase();
extern TCase* git_extension_tcase();
extern TCase* packfile_negotiation_tcase();
extern TCase* packfile_transfer_tcase();
extern TCase* pkt_line_tcase();
extern TCase* reference_discovery_tcase();
extern TCase* report_status_tcase();
extern TCase* update_request_tcase();

Suite* git_extension_suite() {
  Suite* s = suite_create("git extension");

  suite_add_tcase(s, capabilities_tcase());
  suite_add_tcase(s, checksum_tcase());
  suite_add_tcase(s, compress_tcase());
  suite_add_tcase(s, git_extension_tcase());
  suite_add_tcase(s, packfile_negotiation_tcase());
  suite_add_tcase(s, packfile_transfer_tcase());
  suite_add_tcase(s, pkt_line_tcase());
  suite_add_tcase(s, reference_discovery_tcase());
  suite_add_tcase(s, report_status_tcase());
  suite_add_tcase(s, update_request_tcase());

  return s;
}

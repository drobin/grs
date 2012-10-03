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
#include <unistd.h>

extern TCase* acl_tcase();
extern TCase* buffer_tcase();
extern TCase* grs_tcase();
extern TCase* process_tcase();
extern TCase* session_tcase();

static Suite* grs_suite() {
  Suite* s = suite_create("grs_test");

  suite_add_tcase(s, acl_tcase());
  suite_add_tcase(s, buffer_tcase());
  suite_add_tcase(s, grs_tcase());
  suite_add_tcase(s, process_tcase());
  suite_add_tcase(s, session_tcase());

  return s;
}

int main(int argc, char** argv) {
  int nfailed;
  int c;
  int enable_debug = 0;

  while ((c = getopt(argc, argv, "d")) != -1) {
    switch (c) {
      case 'd':
        enable_debug = 1;
        break;
      default:
        return 1;
    }
  }

  Suite* s = grs_suite();
  SRunner* sr = srunner_create(s);

  if (enable_debug) {
    srunner_set_fork_status(sr, CK_NOFORK);
  }

  srunner_run_all(sr, CK_NORMAL);

  nfailed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (nfailed == 0) ? 0 : 1;
}

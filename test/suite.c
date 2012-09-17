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

extern TCase* grs_tcase();
extern TCase* process_tcase();
extern TCase* session_tcase();

static Suite* grs_suite() {
  Suite* s = suite_create("grs_test");

  suite_add_tcase(s, grs_tcase());
  suite_add_tcase(s, process_tcase());
  suite_add_tcase(s, session_tcase());

  return s;
}

int main(int argc, char** argv) {
  int nfailed;

  Suite* s = grs_suite();
  SRunner* sr = srunner_create(s);

  #ifdef ENABLE_DEBUG
  srunner_set_fork_status(sr, CK_NOFORK);
  #endif

  srunner_run_all(sr, CK_NORMAL);

  nfailed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (nfailed == 0) ? 0 : 1;
}

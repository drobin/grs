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

extern Suite* libgrs_suite();

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

  Suite* s = libgrs_suite();
  SRunner* sr = srunner_create(s);

  if (enable_debug) {
    srunner_set_fork_status(sr, CK_NOFORK);
  }

  srunner_run_all(sr, CK_NORMAL);

  nfailed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (nfailed == 0) ? 0 : 1;
}

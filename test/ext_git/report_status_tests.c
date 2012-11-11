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

#include "../../src/extension/git/protocol.h"

static buffer_t out;

static void setup() {
  fail_unless((out = buffer_create()) != NULL);
}

static void teardown() {
  fail_unless(buffer_destroy(out) == 0);
  out = NULL;
}

START_TEST(null_repository) {
  fail_unless(report_status(NULL, out) == -1);
}
END_TEST

START_TEST(null_out) {
  fail_unless(report_status("XXX", NULL) == -1);
}
END_TEST

START_TEST(flush_pkt) {
  fail_unless(report_status("XXX", out) == 0);
  fail_unless(buffer_get_size(out) == 14);
  fail_unless(memcmp(buffer_get_data(out), "000eunpack ok\n", 14) == 0);
}
END_TEST

TCase* report_status_tcase() {
  TCase* tc = tcase_create("status report");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, null_repository);
  tcase_add_test(tc, null_out);
  tcase_add_test(tc, flush_pkt);

  return tc;
}

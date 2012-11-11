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

static buffer_t in;

static void setup() {
  fail_unless((in = buffer_create()) != NULL);
}

static void teardown() {
  fail_unless(buffer_destroy(in) == 0);
  in = NULL;
}

START_TEST(null_repository) {
  fail_unless(update_request(NULL, in) == -1);
}
END_TEST

START_TEST(null_in) {
  fail_unless(update_request("XXX", NULL) == -1);
}
END_TEST

START_TEST(flush_pkt) {
  buffer_append(in, "0000", 4);
  fail_unless(update_request("XXX", in) == 0);
}
END_TEST

START_TEST(unsupported_line) {
  buffer_append(in, "0008XXXX", 8);
  fail_unless(update_request("XXX", in) == -1);
}
END_TEST

TCase* update_request_tcase() {
  TCase* tc = tcase_create("update request");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, null_repository);
  tcase_add_test(tc, null_in);
  tcase_add_test(tc, flush_pkt);
  tcase_add_test(tc, unsupported_line);

  return tc;
}

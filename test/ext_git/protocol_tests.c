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

START_TEST(reference_discovery_null_out_buffer) {
  buffer_t err;

  fail_unless((err = buffer_create()) != NULL);
  fail_unless(reference_discovery(NULL, err) == -1);
  fail_unless(buffer_destroy(err) == 0);
}
END_TEST

START_TEST(reference_discovery_null_err_buffer) {
  buffer_t out;

  fail_unless((out = buffer_create()) != NULL);
  fail_unless(reference_discovery(out, NULL) == -1);
  fail_unless(buffer_destroy(out) == 0);
}
END_TEST

START_TEST(reference_discovery_empty) {
  buffer_t out, err;

  fail_unless((out = buffer_create()) != NULL);
  fail_unless((err = buffer_create()) != NULL);

  fail_unless(reference_discovery(out, err) == 0);

  fail_unless(buffer_get_size(out) == 4);
  fail_unless(strncmp(buffer_get_data(out), "0000", 4) == 0);
  fail_unless(buffer_get_size(err) == 0);

  fail_unless(buffer_destroy(out) == 0);
  fail_unless(buffer_destroy(err) == 0);
}
END_TEST

TCase* git_protocol_tcase() {
  TCase* tc = tcase_create("git protocol");

  tcase_add_test(tc, reference_discovery_null_out_buffer);
  tcase_add_test(tc, reference_discovery_null_err_buffer);
  tcase_add_test(tc, reference_discovery_empty);

  return tc;
}

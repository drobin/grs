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

#include "../../src/libgrsd/buffer.h"

static buffer_t buffer;

static void setup() {
  fail_unless((buffer = buffer_create()) != NULL);
}

static void teardown() {
  fail_unless(buffer_destroy(buffer) == 0);
  buffer = NULL;
}

START_TEST(destroy_null_buffer) {
  fail_unless(buffer_destroy(NULL) == -1);
}
END_TEST

START_TEST(get_capacity_null_buffer) {
  fail_unless(buffer_get_capacity(NULL) == -1);
}
END_TEST

START_TEST(get_capacity_initial) {
  fail_unless(buffer_get_capacity(buffer) == 0);
}
END_TEST

START_TEST(get_size_null_buffer) {
  fail_unless(buffer_get_size(NULL) == -1);
}
END_TEST

START_TEST(get_size_empty_buffer) {
  fail_unless(buffer_get_size(buffer) == 0);
}
END_TEST

TCase* buffer_tcase() {
  TCase* tc = tcase_create("buffer");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, destroy_null_buffer);
  tcase_add_test(tc, get_capacity_null_buffer);
  tcase_add_test(tc, get_capacity_initial);
  tcase_add_test(tc, get_size_null_buffer);
  tcase_add_test(tc, get_size_empty_buffer);

  return tc;
}

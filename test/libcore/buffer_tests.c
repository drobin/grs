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

#include "../../src/libgrs/core/buffer.h"

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

START_TEST(get_data_null_buffer) {
  fail_unless(buffer_get_data(NULL) == NULL);
}
END_TEST

START_TEST(append_null_buffer) {
  fail_unless(buffer_append(NULL, "123", 3) == -1);
}
END_TEST

START_TEST(append_null_data) {
  fail_unless(buffer_append(buffer, NULL, 3) == -1);
}
END_TEST

START_TEST(append_no_data) {
  fail_unless(buffer_append(buffer, "123", 0) == 0);
  fail_unless(buffer_get_capacity(buffer) == 0);
  fail_unless(buffer_get_size(buffer) == 0);
}
END_TEST

START_TEST(append_with_data) {
  fail_unless(buffer_append(buffer, "123", 3) == 0);
  fail_unless(buffer_get_capacity(buffer) == 3);
  fail_unless(buffer_get_size(buffer) == 3);
  fail_unless(memcmp(buffer_get_data(buffer), "123", 3) == 0);
}
END_TEST

START_TEST(append_multiple) {
  fail_unless(buffer_append(buffer, "123", 3) == 0);
  fail_unless(buffer_get_capacity(buffer) == 3);
  fail_unless(buffer_get_size(buffer) == 3);
  fail_unless(memcmp(buffer_get_data(buffer), "123", 3) == 0);

  fail_unless(buffer_append(buffer, "45", 2) == 0);
  fail_unless(buffer_get_capacity(buffer) == 5);
  fail_unless(buffer_get_size(buffer) == 5);
  fail_unless(memcmp(buffer_get_data(buffer), "12345", 5) == 0);
}
END_TEST

START_TEST(remove_null_buffer) {
  fail_unless(buffer_remove(NULL, 3) == -1);
}
END_TEST

START_TEST(remove_not_empty) {
  fail_unless(buffer_append(buffer, "12345", 5) == 0);
  fail_unless(buffer_get_capacity(buffer) == 5);
  fail_unless(buffer_get_size(buffer) == 5);
  fail_unless(memcmp(buffer_get_data(buffer), "12345", 5) == 0);

  fail_unless(buffer_remove(buffer, 3) == 0);
  fail_unless(buffer_get_capacity(buffer) == 5);
  fail_unless(buffer_get_size(buffer) == 2);
  fail_unless(memcmp(buffer_get_data(buffer), "45", 2) == 0);
}
END_TEST

START_TEST(remove_empty) {
  fail_unless(buffer_append(buffer, "12345", 5) == 0);
  fail_unless(buffer_get_capacity(buffer) == 5);
  fail_unless(buffer_get_size(buffer) == 5);
  fail_unless(memcmp(buffer_get_data(buffer), "12345", 5) == 0);

  fail_unless(buffer_remove(buffer, 5) == 0);
  fail_unless(buffer_get_capacity(buffer) == 5);
  fail_unless(buffer_get_size(buffer) == 0);
}
END_TEST

START_TEST(remove_over_size) {
  fail_unless(buffer_append(buffer, "12345", 5) == 0);
  fail_unless(buffer_get_capacity(buffer) == 5);
  fail_unless(buffer_get_size(buffer) == 5);
  fail_unless(memcmp(buffer_get_data(buffer), "12345", 5) == 0);

  fail_unless(buffer_remove(buffer, 50) == 0);
  fail_unless(buffer_get_capacity(buffer) == 5);
  fail_unless(buffer_get_size(buffer) == 0);
}
END_TEST

START_TEST(add_remove) {
  fail_unless(buffer_append(buffer, "123", 3) == 0);
  fail_unless(buffer_get_capacity(buffer) == 3);
  fail_unless(buffer_get_size(buffer) == 3);
  fail_unless(memcmp(buffer_get_data(buffer), "123", 3) == 0);

  fail_unless(buffer_remove(buffer, 2) == 0);
  fail_unless(buffer_get_capacity(buffer) == 3);
  fail_unless(buffer_get_size(buffer) == 1);
  fail_unless(memcmp(buffer_get_data(buffer), "3", 1) == 0);

  fail_unless(buffer_append(buffer, "456", 3) == 0);
  fail_unless(buffer_get_capacity(buffer) == 4);
  fail_unless(buffer_get_size(buffer) == 4);
  fail_unless(memcmp(buffer_get_data(buffer), "3456", 4) == 0);

  fail_unless(buffer_remove(buffer, 2) == 0);
  fail_unless(buffer_get_capacity(buffer) == 4);
  fail_unless(buffer_get_size(buffer) == 2);
  fail_unless(memcmp(buffer_get_data(buffer), "56", 2) == 0);

  fail_unless(buffer_append(buffer, "7", 1) == 0);
  fail_unless(buffer_get_capacity(buffer) == 4);
  fail_unless(buffer_get_size(buffer) == 3);
  fail_unless(memcmp(buffer_get_data(buffer), "567", 3) == 0);
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
  tcase_add_test(tc, get_data_null_buffer);
  tcase_add_test(tc, append_null_buffer);
  tcase_add_test(tc, append_null_data);
  tcase_add_test(tc, append_no_data);
  tcase_add_test(tc, append_with_data);
  tcase_add_test(tc, append_multiple);
  tcase_add_test(tc, remove_null_buffer);
  tcase_add_test(tc, remove_not_empty);
  tcase_add_test(tc, remove_empty);
  tcase_add_test(tc, remove_over_size);
  tcase_add_test(tc, add_remove);

  return tc;
}

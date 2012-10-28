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
#include <stdint.h>

#include "../../src/libgrs/binbuf.h"

struct test_buf {
  uint32_t i;
};

static binbuf_t buffer;

static void setup() {
  fail_unless((buffer = binbuf_create(sizeof(struct test_buf))) != NULL);
}

static void teardown() {
  fail_unless(binbuf_destroy(buffer) == 0);
  buffer = NULL;
}

START_TEST(destroy_null_buf) {
  fail_unless(binbuf_destroy(NULL) == -1);
}
END_TEST

START_TEST(get_size_of_null_buf) {
  fail_unless(binbuf_get_size_of(NULL) == -1);
}
END_TEST

START_TEST(get_size_of) {
  binbuf_t buf;

  fail_unless((buf = binbuf_create(8)) != NULL);
  fail_unless(binbuf_get_size_of(buf) == 8);
  fail_unless(binbuf_destroy(buf) == 0);
}
END_TEST

START_TEST(get_size_null_buf) {
  fail_unless(binbuf_get_size(NULL) == -1);
}
END_TEST

START_TEST(get_size_initial) {
  fail_unless(binbuf_get_size(buffer) == 0);
}
END_TEST

START_TEST(get_capacity_null_buf) {
  fail_unless(binbuf_get_capacity(NULL) == -1);
}
END_TEST

START_TEST(get_capacity_initial) {
  fail_unless(binbuf_get_capacity(buffer) == 0);
}
END_TEST

START_TEST(get_null_buf) {
  fail_unless(binbuf_add(NULL) == NULL);
}
END_TEST

START_TEST(get_empty) {
  fail_unless(binbuf_get(buffer, 0) == NULL);
  fail_unless(binbuf_get(buffer, 1) == NULL);
}
END_TEST

START_TEST(find_null_buf) {
  struct test_buf cmp;
  fail_unless(binbuf_find(NULL, &cmp, sizeof(struct test_buf)) == -1);
}
END_TEST

START_TEST(find_null_cmp) {
  fail_unless(binbuf_find(buffer, NULL, sizeof(struct test_buf)) == -1);
}
END_TEST

START_TEST(find_empty) {
  struct test_buf cmp;
  cmp.i = 1;

  fail_unless(binbuf_find(buffer, &cmp, sizeof(struct test_buf)) == -1);
}
END_TEST

START_TEST(find_found) {
  struct test_buf* entry;
  uint32_t cmp;

  fail_unless((entry = binbuf_add(buffer)) != NULL);
  entry->i = 1;
  fail_unless((entry = binbuf_add(buffer)) != NULL);
  entry->i = 2;
  fail_unless(binbuf_get_size(buffer) == 2);

  cmp = 1;
  fail_unless(binbuf_find(buffer, &cmp, sizeof(uint32_t)) == 0);

  cmp = 2;
  fail_unless(binbuf_find(buffer, &cmp, sizeof(uint32_t)) == 1);
}
END_TEST

START_TEST(find_found_smaller_size) {
  struct test_buf* entry;
  uint32_t cmp;

  fail_unless((entry = binbuf_add(buffer)) != NULL);
  entry->i = ntohl(0x1000100);
  fail_unless((entry = binbuf_add(buffer)) != NULL);
  entry->i = ntohl(0x3000101);
  fail_unless(binbuf_get_size(buffer) == 2);

  cmp = ntohl(0x3000103);
  fail_unless(binbuf_find(buffer, &cmp, sizeof(uint32_t)) == -1);
  fail_unless(binbuf_find(buffer, &cmp, sizeof(uint16_t)) == 1);
}
END_TEST

START_TEST(find_not_found) {
  struct test_buf* entry;
  uint32_t cmp;

  fail_unless((entry = binbuf_add(buffer)) != NULL);
  entry->i = 1;
  fail_unless((entry = binbuf_add(buffer)) != NULL);
  entry->i = 2;
  fail_unless(binbuf_get_size(buffer) == 2);

  cmp = 3;
  fail_unless(binbuf_find(buffer, &cmp, sizeof(uint32_t)) == -1);
}
END_TEST

START_TEST(add_null_buf) {
  fail_unless(binbuf_add(NULL) == NULL);
}
END_TEST

START_TEST(add_success) {
  struct test_buf* e1;
  const struct test_buf* e2;

  fail_unless((e1 = binbuf_add(buffer)) != NULL);
  e1->i = 1;

  fail_unless(binbuf_get_size(buffer) == 1);
  fail_unless(binbuf_get_capacity(buffer) == 1);
  fail_unless((e2 = binbuf_get(buffer, 0)) == e1);
  fail_unless(e2->i == 1);

  fail_unless((e1 = binbuf_add(buffer)) != NULL);
  e1->i = 2;

  fail_unless(binbuf_get_size(buffer) == 2);
  fail_unless(binbuf_get_capacity(buffer) == 2);
  fail_unless((e2 = binbuf_get(buffer, 1)) == e1);
  fail_unless(e2->i == 2);
}
END_TEST

START_TEST(add_out_of_range) {
  fail_unless(binbuf_add(buffer) != NULL);
  fail_unless(binbuf_add(buffer) != NULL);
  fail_unless(binbuf_get_size(buffer) == 2);
  fail_unless(binbuf_get(buffer, 2) == NULL);
}
END_TEST

TCase* binbuf_tcase() {
  TCase* tc = tcase_create("binbuf");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, destroy_null_buf);
  tcase_add_test(tc, get_size_of_null_buf);
  tcase_add_test(tc, get_size_of);
  tcase_add_test(tc, get_size_null_buf);
  tcase_add_test(tc, get_size_initial);
  tcase_add_test(tc, get_capacity_null_buf);
  tcase_add_test(tc, get_capacity_initial);
  tcase_add_test(tc, get_null_buf);
  tcase_add_test(tc, get_empty);
  tcase_add_test(tc, find_null_buf);
  tcase_add_test(tc, find_null_cmp);
  tcase_add_test(tc, find_empty);
  tcase_add_test(tc, find_found);
  tcase_add_test(tc, find_found_smaller_size);
  tcase_add_test(tc, find_not_found);
  tcase_add_test(tc, add_null_buf);
  tcase_add_test(tc, add_success);
  tcase_add_test(tc, add_out_of_range);

  return tc;
}

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

#include "../../src/libgrs/buffer.h"
#include "../../src/extension/git/pkt_line.h"

static buffer_t src;
static buffer_t dest;

static void setup() {
  fail_unless((src = buffer_create()) != NULL);
  fail_unless((dest = buffer_create()) != NULL);
}

static void teardown() {
  fail_unless(buffer_destroy(src) == 0);
  fail_unless(buffer_destroy(dest) == 0);
  src = NULL;
  dest = NULL;
}

START_TEST(pkt_line_read_null_src) {
  fail_unless(pkt_line_read(NULL, dest) == -1);
}
END_TEST

START_TEST(pkt_line_read_null_dest) {
  fail_unless(pkt_line_read(src, NULL) == -1);
}
END_TEST

START_TEST(pkt_line_read_incomplete_length) {
  fail_unless(buffer_append(src, "123", 3) == 0);
  fail_unless(pkt_line_read(src, dest) == -1);

  fail_unless(buffer_get_size(src) == 3);
  fail_unless(strncmp(buffer_get_data(src), "123", 3) == 0);
  fail_unless(buffer_get_size(dest) == 0);
}
END_TEST

START_TEST(pkt_line_read_incomplete_payload) {
  fail_unless(buffer_append(src, "0006x", 5) == 0);
  fail_unless(pkt_line_read(src, dest) == -1);

  fail_unless(buffer_get_size(src) == 5);
  fail_unless(strncmp(buffer_get_data(src), "0006x", 5) == 0);
  fail_unless(buffer_get_size(dest) == 0);
}
END_TEST

START_TEST(pkt_line_read_success) {
  fail_unless(buffer_append(src, "0006xxabc", 9) == 0);
  fail_unless(pkt_line_read(src, dest) == 0);

  fail_unless(buffer_get_size(src) == 3);
  fail_unless(strncmp(buffer_get_data(src), "abc", 3) == 0);
  fail_unless(buffer_get_size(dest) == 2);
  fail_unless(strncmp(buffer_get_data(dest), "xx", 2) == 0);
}
END_TEST

START_TEST(pkt_line_read_empty) {
  fail_unless(buffer_append(src, "0004", 4) == 0);
  fail_unless(pkt_line_read(src, dest) == -1);

  fail_unless(buffer_get_size(src) == 4);
  fail_unless(strncmp(buffer_get_data(src), "0004", 4) == 0);
  fail_unless(buffer_get_size(dest) == 0);
}
END_TEST

START_TEST(pkt_line_read_flush_pkt) {
  fail_unless(buffer_append(src, "0000abc", 7) == 0);
  fail_unless(pkt_line_read(src, dest) == 0);

  fail_unless(buffer_get_size(src) == 3);
  fail_unless(strncmp(buffer_get_data(src), "abc", 3) == 0);
  fail_unless(buffer_get_size(dest) == 0);
}
END_TEST

START_TEST(pkt_line_write_null_src) {
  fail_unless(pkt_line_write(NULL, dest) == -1);
}
END_TEST

START_TEST(pkt_line_write_null_dest) {
  fail_unless(pkt_line_write(src, NULL) == -1);
}
END_TEST

START_TEST(pkt_line_write_flush_pkt) {
  fail_unless(pkt_line_write(src, dest) == 0);
  fail_unless(buffer_get_size(src) == 0);
  fail_unless(buffer_get_size(dest) == 4);
  fail_unless(strncmp(buffer_get_data(dest), "0000", 4) == 0);
}
END_TEST

START_TEST(pkt_line_write_success) {
  fail_unless(buffer_append(src, "abcdefg", 7) == 0);
  fail_unless(pkt_line_write(src, dest) == 0);

  fail_unless(buffer_get_size(src) == 7);
  fail_unless(strncmp(buffer_get_data(src), "abcdefg", 7) == 0);
  fail_unless(buffer_get_size(dest) == 11);
  fail_unless(strncmp(buffer_get_data(dest), "000babcdefg", 11) == 0);
}
END_TEST

TCase* pkt_line_tcase() {
  TCase* tc = tcase_create("pkt_line");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, pkt_line_read_null_src);
  tcase_add_test(tc, pkt_line_read_null_dest);
  tcase_add_test(tc, pkt_line_read_incomplete_length);
  tcase_add_test(tc, pkt_line_read_incomplete_payload);
  tcase_add_test(tc, pkt_line_read_success);
  tcase_add_test(tc, pkt_line_read_empty);
  tcase_add_test(tc, pkt_line_read_flush_pkt);
  tcase_add_test(tc, pkt_line_write_null_src);
  tcase_add_test(tc, pkt_line_write_null_dest);
  tcase_add_test(tc, pkt_line_write_flush_pkt);
  tcase_add_test(tc, pkt_line_write_success);

  return tc;
}

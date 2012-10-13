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

static buffer_t buffer;

static void setup() {
  fail_unless((buffer = buffer_create()) != NULL);
}

static void teardown() {
  fail_unless(buffer_destroy(buffer) == 0);
  buffer = NULL;
}

START_TEST(pkt_line_create_flush_pkt) {
  struct pkt_line* line;

  fail_unless((line = pkt_line_create(0, NULL)) != NULL);
  fail_unless(line->len == 0);
  fail_unless(pkt_line_destroy(line) == 0);
}
END_TEST

START_TEST(pkt_line_create_null_payload) {
  struct pkt_line* line;

  fail_unless((line = pkt_line_create(3, NULL)) == NULL);
}
END_TEST

START_TEST(pkt_line_create_success) {
  struct pkt_line* line;

  fail_unless((line = pkt_line_create(3, "abc")) != NULL);
  fail_unless(line->len == 3);
  fail_unless(strncmp(line->payload, "abc", 3) == 0);
  fail_unless(pkt_line_destroy(line) == 0);
}
END_TEST

START_TEST(pkt_line_destroy_null_buffer) {
  fail_unless(pkt_line_destroy(NULL) == -1);
}
END_TEST

START_TEST(pkt_line_read_null_buffer) {
  fail_unless(pkt_line_read(NULL) == NULL);
}
END_TEST

START_TEST(pkt_line_read_incomplete_length) {
  fail_unless(buffer_append(buffer, "123", 3) == 0);
  fail_unless(pkt_line_read(buffer) == NULL);

  fail_unless(buffer_get_size(buffer) == 3);
  fail_unless(strncmp(buffer_get_data(buffer), "123", 3) == 0);
}
END_TEST

START_TEST(pkt_line_read_incomplete_payload) {
  fail_unless(buffer_append(buffer, "0006x", 5) == 0);
  fail_unless(pkt_line_read(buffer) == NULL);

  fail_unless(buffer_get_size(buffer) == 5);
  fail_unless(strncmp(buffer_get_data(buffer), "0006x", 5) == 0);
}
END_TEST

START_TEST(pkt_line_read_success) {
  struct pkt_line* line;

  fail_unless(buffer_append(buffer, "0006xxabc", 9) == 0);
  fail_unless((line = pkt_line_read(buffer)) != NULL);
  fail_unless(line->len == 2);
  fail_unless(strncmp(line->payload, "xx", 2) == 0);

  fail_unless(buffer_get_size(buffer) == 3);
  fail_unless(strncmp(buffer_get_data(buffer), "abc", 3) == 0);

  fail_unless(pkt_line_destroy(line) == 0);
}
END_TEST

START_TEST(pkt_line_read_empty) {
  fail_unless(buffer_append(buffer, "0004", 4) == 0);
  fail_unless(pkt_line_read(buffer) == NULL);

  fail_unless(buffer_get_size(buffer) == 4);
  fail_unless(strncmp(buffer_get_data(buffer), "0004", 4) == 0);
}
END_TEST

START_TEST(pkt_line_read_flush_pkt) {
  struct pkt_line* line;

  fail_unless(buffer_append(buffer, "0000abc", 7) == 0);
  fail_unless((line = pkt_line_read(buffer)) != NULL);
  fail_unless(line->len == 0);

  fail_unless(buffer_get_size(buffer) == 3);
  fail_unless(strncmp(buffer_get_data(buffer), "abc", 3) == 0);

  fail_unless(pkt_line_destroy(line) == 0);
}
END_TEST

START_TEST(pkt_line_write_null_line) {
  fail_unless(pkt_line_write(NULL, buffer) == -1);
}
END_TEST

START_TEST(pkt_line_write_null_buffer) {
  struct pkt_line* line;

  fail_unless((line = pkt_line_create(0, NULL)) != NULL);
  fail_unless(pkt_line_write(line, NULL) == -1);
  fail_unless(pkt_line_destroy(line) == 0);
}
END_TEST

START_TEST(pkt_line_write_flush_pkt) {
  struct pkt_line* line;

  fail_unless((line = pkt_line_create(0, NULL)) != NULL);
  fail_unless(pkt_line_write(line, buffer) == 0);
  fail_unless(pkt_line_destroy(line) == 0);
  fail_unless(buffer_get_size(buffer) == 4);
  fail_unless(strncmp(buffer_get_data(buffer), "0000", 4) == 0);
}
END_TEST

START_TEST(pkt_line_write_success) {
  struct pkt_line* line;

  fail_unless((line = pkt_line_create(7, "abcdefg")) != NULL);
  fail_unless(pkt_line_write(line, buffer) == 0);
  fail_unless(pkt_line_destroy(line) == 0);
  fail_unless(buffer_get_size(buffer) == 11);
  fail_unless(strncmp(buffer_get_data(buffer), "000babcdefg", 11) == 0);
}
END_TEST

TCase* pkt_line_tcase() {
  TCase* tc = tcase_create("pkt_line");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, pkt_line_create_flush_pkt);
  tcase_add_test(tc, pkt_line_create_null_payload);
  tcase_add_test(tc, pkt_line_create_success);
  tcase_add_test(tc, pkt_line_destroy_null_buffer);
  tcase_add_test(tc, pkt_line_read_null_buffer);
  tcase_add_test(tc, pkt_line_read_incomplete_length);
  tcase_add_test(tc, pkt_line_read_incomplete_payload);
  tcase_add_test(tc, pkt_line_read_success);
  tcase_add_test(tc, pkt_line_read_empty);
  tcase_add_test(tc, pkt_line_read_flush_pkt);
  tcase_add_test(tc, pkt_line_write_null_line);
  tcase_add_test(tc, pkt_line_write_null_buffer);
  tcase_add_test(tc, pkt_line_write_flush_pkt);
  tcase_add_test(tc, pkt_line_write_success);

  return tc;
}

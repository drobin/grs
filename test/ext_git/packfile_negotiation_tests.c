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
#include <stdlib.h>

#include "../../src/extension/git/protocol.h"

static struct packfile_negotiation_data data;
static buffer_t in;
static buffer_t out;
static buffer_t err;

static void setup() {
  memset(&data, 0, sizeof(struct packfile_negotiation_data));
  fail_unless((in = buffer_create()) != NULL);
  fail_unless((out = buffer_create()) != NULL);
  fail_unless((err = buffer_create()) != NULL);
}

static void teardown() {
  if (data.phase > packfile_negotiation_prepare &&
      data.phase < packfile_negotiation_finished) {
    data.phase = packfile_negotiation_finished;
    buffer_clear(in);
    buffer_append(in, "0000", 4);
    fail_unless(packfile_negotiation(in, out, &data) == 0);
  }

  fail_unless(buffer_destroy(in) == 0);
  fail_unless(buffer_destroy(out) == 0);
  fail_unless(buffer_destroy(err) == 0);
  in = NULL;
  out = NULL;
  err = NULL;
}

START_TEST(null_in) {
  fail_unless(packfile_negotiation(NULL, out, &data) == -1);
}
END_TEST

START_TEST(null_out) {
  fail_unless(packfile_negotiation(in, NULL, &data) == -1);
}
END_TEST

START_TEST(null_data) {
  fail_unless(packfile_negotiation(in, out, NULL) == -1);
}
END_TEST

START_TEST(upload_request_no_in_data) {
  fail_unless(packfile_negotiation(in, out, &data) == 1);
}
END_TEST

START_TEST(upload_request_unknown_request) {
  buffer_append(in, "0007xxx", 7);
  fail_unless(packfile_negotiation(in, out, &data) == -1);
}
END_TEST

START_TEST(upload_request_one_want) {
  buffer_append(in, "0032want 0123456789012345678901234567890123456789\n", 50);
  fail_unless(packfile_negotiation(in, out, &data) == 1);
  fail_unless(binbuf_get_size(data.want_list) == 1);
  fail_unless(memcmp(binbuf_get(data.want_list, 0),
                     "0123456789012345678901234567890123456789", 40) == 0);
  fail_unless(binbuf_get_size(data.shallow_list) == 0);
  fail_unless(data.depth == -1);
}
END_TEST

START_TEST(upload_request_two_wants) {
  buffer_append(in, "0032want 0123456789012345678901234567890123456789\n", 50);
  buffer_append(in, "0032want 9876543210987654321098765432109876543210\n", 50);
  fail_unless(packfile_negotiation(in, out, &data) == 1);
  fail_unless(binbuf_get_size(data.want_list) == 2);
  fail_unless(memcmp(binbuf_get(data.want_list, 0),
                     "0123456789012345678901234567890123456789", 40) == 0);
  fail_unless(memcmp(binbuf_get(data.want_list, 1),
                     "9876543210987654321098765432109876543210", 40) == 0);
  fail_unless(binbuf_get_size(data.shallow_list) == 0);
  fail_unless(data.depth == -1);
}
END_TEST

START_TEST(upload_request_shallow) {
  buffer_append(in, "0032want 0123456789012345678901234567890123456789\n", 50);
  buffer_append(in, "0034shallow 9876543210987654321098765432109876543210", 52);
  fail_unless(packfile_negotiation(in, out, &data) == 1);
  fail_unless(binbuf_get_size(data.want_list) == 1);
  fail_unless(memcmp(binbuf_get(data.want_list, 0),
                     "0123456789012345678901234567890123456789", 40) == 0);
  fail_unless(binbuf_get_size(data.shallow_list) == 1);
  fail_unless(memcmp(binbuf_get(data.shallow_list, 0),
                     "9876543210987654321098765432109876543210", 40) == 0);
  fail_unless(data.depth == -1);
}
END_TEST

START_TEST(upload_request_shallow_depth) {
  buffer_append(in, "0032want 0123456789012345678901234567890123456789\n", 50);
  buffer_append(in, "0034shallow 9876543210987654321098765432109876543210", 52);
  buffer_append(in, "000cdeepen 7", 12);
  fail_unless(packfile_negotiation(in, out, &data) == 1);
  fail_unless(binbuf_get_size(data.want_list) == 1);
  fail_unless(memcmp(binbuf_get(data.want_list, 0),
                     "0123456789012345678901234567890123456789", 40) == 0);
  fail_unless(binbuf_get_size(data.shallow_list) == 1);
  fail_unless(memcmp(binbuf_get(data.shallow_list, 0),
                     "9876543210987654321098765432109876543210", 40) == 0);
  fail_unless(data.depth == 7);
}
END_TEST

START_TEST(upload_request_skipped_shallow_depth) {
  buffer_append(in, "0032want 0123456789012345678901234567890123456789\n", 50);
  buffer_append(in, "000cdeepen 7", 12);
  fail_unless(packfile_negotiation(in, out, &data) == 1);
  fail_unless(binbuf_get_size(data.want_list) == 1);
  fail_unless(memcmp(binbuf_get(data.want_list, 0),
                     "0123456789012345678901234567890123456789", 40) == 0);
  fail_unless(binbuf_get_size(data.shallow_list) == 0);
  fail_unless(data.depth == 7);
}
END_TEST

TCase* packfile_negotiation_tcase() {
  TCase* tc = tcase_create("packfile negotiation");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, null_in);
  tcase_add_test(tc, null_out);
  tcase_add_test(tc, null_data);
  tcase_add_test(tc, upload_request_no_in_data);
  tcase_add_test(tc, upload_request_unknown_request);
  tcase_add_test(tc, upload_request_one_want);
  tcase_add_test(tc, upload_request_two_wants);
  tcase_add_test(tc, upload_request_shallow);
  tcase_add_test(tc, upload_request_shallow_depth);
  tcase_add_test(tc, upload_request_skipped_shallow_depth);

  return tc;
}

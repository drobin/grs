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

static int no_refs_stub(const char* repository, binbuf_t refs) {
  return 0;
}

static int refs_stub(const char* repository, binbuf_t refs) {
  struct git_ref* ref;

  ref = binbuf_add(refs);
  strlcpy(ref->obj_id, "objid1", sizeof(ref->obj_id));
  strlcpy(ref->ref_name, "foo", sizeof(ref->ref_name));

  ref = binbuf_add(refs);
  strlcpy(ref->obj_id, "objid2", sizeof(ref->obj_id));
  strlcpy(ref->ref_name, "bar", sizeof(ref->ref_name));

  return 0;
}

static int failed_stub(const char* repository, binbuf_t refs) {
  return -1;
}

static buffer_t in;
static buffer_t out;
static buffer_t err;

static void setup() {
  fail_unless((in = buffer_create()) != NULL);
  fail_unless((out = buffer_create()) != NULL);
  fail_unless((err = buffer_create()) != NULL);
}

static void teardown() {
  fail_unless(buffer_destroy(in) == 0);
  fail_unless(buffer_destroy(out) == 0);
  fail_unless(buffer_destroy(err) == 0);
  in = NULL;
  out = NULL;
  err = NULL;
}

START_TEST(reference_discovery_null_out_repository) {
  fail_unless(reference_discovery(NULL, out, err, no_refs_stub) == -1);
}
END_TEST

START_TEST(reference_discovery_null_out_buffer) {
  fail_unless(reference_discovery("xxx", NULL, err, no_refs_stub) == -1);
}
END_TEST

START_TEST(reference_discovery_null_err_buffer) {
  fail_unless(reference_discovery("xxx", out, NULL, no_refs_stub) == -1);
}
END_TEST

START_TEST(reference_discovery_null_refs) {
  fail_unless(reference_discovery("xxx", out, err, NULL) == -1);
}
END_TEST

START_TEST(reference_discovery_empty) {
  fail_unless(reference_discovery("xxx", out, err, no_refs_stub) == 0);
  fail_unless(buffer_get_size(out) == 4);
  fail_unless(strncmp(buffer_get_data(out), "0000", 4) == 0);
  fail_unless(buffer_get_size(err) == 0);
}
END_TEST

START_TEST(reference_discovery_fetch_failed) {
  fail_unless(reference_discovery("xxx", out, err, failed_stub) == -1);
  fail_unless(buffer_get_size(out) == 0);
  fail_unless(buffer_get_size(err) == 0);
}
END_TEST

START_TEST(reference_discovery_fetch) {
  fail_unless(reference_discovery("xxx", out, err, refs_stub) == 0);
  fail_unless(buffer_get_size(out) == 34);
  fail_unless(strncmp(buffer_get_data(out),
                      "000fobjid1 foo\n"
                      "000fobjid2 bar\n"
                      "0000", 34) == 0);
  fail_unless(buffer_get_size(err) == 0);
}
END_TEST

START_TEST(packfile_negotiation_null_in) {
  struct packfile_negotiation_data data;
  fail_unless(packfile_negotiation(NULL, &data) == -1);
}
END_TEST

START_TEST(packfile_negotiation_null_data) {
  fail_unless(packfile_negotiation(in, NULL) == -1);
}
END_TEST

START_TEST(packfile_negotiation_no_in_data) {
  struct packfile_negotiation_data data;

  memset(&data, 0, sizeof(struct packfile_negotiation_data));
  fail_unless(packfile_negotiation(in, &data) == 1);
}
END_TEST

START_TEST(packfile_negotiation_unknown_upload_request) {
  struct packfile_negotiation_data data;

  memset(&data, 0, sizeof(struct packfile_negotiation_data));
  buffer_append(in, "0007xxx", 7);
  fail_unless(packfile_negotiation(in, &data) == -1);
}
END_TEST

START_TEST(packfile_negotiation_one_want) {
  struct packfile_negotiation_data data;

  memset(&data, 0, sizeof(struct packfile_negotiation_data));
  buffer_append(in, "0032want 0123456789012345678901234567890123456789\n", 50);
  buffer_append(in, "0000", 4);
  fail_unless(packfile_negotiation(in, &data) == 0);
  fail_unless(binbuf_get_size(data.want_list) == 1);
  fail_unless(memcmp(binbuf_get(data.want_list, 0),
                     "0123456789012345678901234567890123456789", 40) == 0);
  fail_unless(binbuf_get_size(data.shallow_list) == 0);
  fail_unless(data.depth == -1);
}
END_TEST

START_TEST(packfile_negotiation_two_wants) {
  struct packfile_negotiation_data data;

  memset(&data, 0, sizeof(struct packfile_negotiation_data));
  buffer_append(in, "0032want 0123456789012345678901234567890123456789\n", 50);
  buffer_append(in, "0032want 9876543210987654321098765432109876543210\n", 50);
  buffer_append(in, "0000", 4);
  fail_unless(packfile_negotiation(in, &data) == 0);
  fail_unless(binbuf_get_size(data.want_list) == 2);
  fail_unless(memcmp(binbuf_get(data.want_list, 0),
                     "0123456789012345678901234567890123456789", 40) == 0);
  fail_unless(memcmp(binbuf_get(data.want_list, 1),
                     "9876543210987654321098765432109876543210", 40) == 0);
  fail_unless(binbuf_get_size(data.shallow_list) == 0);
  fail_unless(data.depth == -1);
}
END_TEST

START_TEST(packfile_negotiation_shallow) {
  struct packfile_negotiation_data data;

  memset(&data, 0, sizeof(struct packfile_negotiation_data));
  buffer_append(in, "0032want 0123456789012345678901234567890123456789\n", 50);
  buffer_append(in, "0034shallow 9876543210987654321098765432109876543210", 52);
  buffer_append(in, "0000", 4);
  fail_unless(packfile_negotiation(in, &data) == 0);
  fail_unless(binbuf_get_size(data.want_list) == 1);
  fail_unless(memcmp(binbuf_get(data.want_list, 0),
                     "0123456789012345678901234567890123456789", 40) == 0);
  fail_unless(binbuf_get_size(data.shallow_list) == 1);
  fail_unless(memcmp(binbuf_get(data.shallow_list, 0),
                     "9876543210987654321098765432109876543210", 40) == 0);
  fail_unless(data.depth == -1);
}
END_TEST

START_TEST(packfile_negotiation_shallow_depth) {
  struct packfile_negotiation_data data;

  memset(&data, 0, sizeof(struct packfile_negotiation_data));
  buffer_append(in, "0032want 0123456789012345678901234567890123456789\n", 50);
  buffer_append(in, "0034shallow 9876543210987654321098765432109876543210", 52);
  buffer_append(in, "000cdeepen 7", 12);
  buffer_append(in, "0000", 4);
  fail_unless(packfile_negotiation(in, &data) == 0);
  fail_unless(binbuf_get_size(data.want_list) == 1);
  fail_unless(memcmp(binbuf_get(data.want_list, 0),
                     "0123456789012345678901234567890123456789", 40) == 0);
  fail_unless(binbuf_get_size(data.shallow_list) == 1);
  fail_unless(memcmp(binbuf_get(data.shallow_list, 0),
                     "9876543210987654321098765432109876543210", 40) == 0);
  fail_unless(data.depth == 7);
}
END_TEST

START_TEST(packfile_negotiation_skipped_shallow_depth) {
  struct packfile_negotiation_data data;

  memset(&data, 0, sizeof(struct packfile_negotiation_data));
  buffer_append(in, "0032want 0123456789012345678901234567890123456789\n", 50);
  buffer_append(in, "000cdeepen 7", 12);
  buffer_append(in, "0000", 4);
  fail_unless(packfile_negotiation(in, &data) == 0);
  fail_unless(binbuf_get_size(data.want_list) == 1);
  fail_unless(memcmp(binbuf_get(data.want_list, 0),
                     "0123456789012345678901234567890123456789", 40) == 0);
  fail_unless(binbuf_get_size(data.shallow_list) == 0);
  fail_unless(data.depth == 7);
}
END_TEST

TCase* git_protocol_tcase() {
  TCase* tc = tcase_create("git protocol");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, reference_discovery_null_out_repository);
  tcase_add_test(tc, reference_discovery_null_out_buffer);
  tcase_add_test(tc, reference_discovery_null_err_buffer);
  tcase_add_test(tc, reference_discovery_null_refs);
  tcase_add_test(tc, reference_discovery_empty);
  tcase_add_test(tc, reference_discovery_fetch_failed);
  tcase_add_test(tc, reference_discovery_fetch);
  tcase_add_test(tc, packfile_negotiation_null_in);
  tcase_add_test(tc, packfile_negotiation_null_data);
  tcase_add_test(tc, packfile_negotiation_no_in_data);
  tcase_add_test(tc, packfile_negotiation_unknown_upload_request);
  tcase_add_test(tc, packfile_negotiation_one_want);
  tcase_add_test(tc, packfile_negotiation_two_wants);
  tcase_add_test(tc, packfile_negotiation_shallow);
  tcase_add_test(tc, packfile_negotiation_shallow_depth);
  tcase_add_test(tc, packfile_negotiation_skipped_shallow_depth);

  return tc;
}

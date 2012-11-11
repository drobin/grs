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
static buffer_t err;

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

static void setup() {
  fail_unless((out = buffer_create()) != NULL);
  fail_unless((err = buffer_create()) != NULL);
}

static void teardown() {
  fail_unless(buffer_destroy(out) == 0);
  fail_unless(buffer_destroy(err) == 0);
  out = NULL;
  err = NULL;
}

START_TEST(null_out_repository) {
  fail_unless(reference_discovery(NULL, process_upload_pack, out, err,
                                  no_refs_stub) == -1);
}
END_TEST

START_TEST(null_out_buffer) {
  fail_unless(reference_discovery("xxx", process_upload_pack, NULL, err,
                                  no_refs_stub) == -1);
}
END_TEST

START_TEST(null_err_buffer) {
  fail_unless(reference_discovery("xxx", process_upload_pack, out, NULL,
                                  no_refs_stub) == -1);
}
END_TEST

START_TEST(null_refs) {
  fail_unless(
    reference_discovery("xxx", process_upload_pack, out, err, NULL) == -1);
}
END_TEST

START_TEST(empty_upload_pack) {
  fail_unless(reference_discovery("xxx", process_upload_pack, out, err,
                                  no_refs_stub) == 0);
  fail_unless(buffer_get_size(out) == 4);
  fail_unless(strncmp(buffer_get_data(out), "0000", 4) == 0);
  fail_unless(buffer_get_size(err) == 0);
}
END_TEST

START_TEST(empty_receive_pack) {
  fail_unless(reference_discovery("xxx", process_receive_pack, out, err,
                                  no_refs_stub) == 0);
  fail_unless(buffer_get_size(out) == 4);
  fail_unless(strncmp(buffer_get_data(out), "0000", 4) == 0);
  fail_unless(buffer_get_size(err) == 0);
}
END_TEST

START_TEST(fetch_failed_upload_pack) {
  fail_unless(reference_discovery("xxx", process_upload_pack, out, err,
                                  failed_stub) == -1);
  fail_unless(buffer_get_size(out) == 0);
  fail_unless(buffer_get_size(err) == 0);
}
END_TEST

START_TEST(fetch_failed_receive_pack) {
  fail_unless(reference_discovery("xxx", process_receive_pack, out, err,
                                  failed_stub) == -1);
  fail_unless(buffer_get_size(out) == 0);
  fail_unless(buffer_get_size(err) == 0);
}
END_TEST

START_TEST(fetch_upload_pack) {
  fail_unless(reference_discovery("xxx", process_upload_pack, out, err,
                                  refs_stub) == 0);
  fail_unless(buffer_get_size(out) == 34);
  fail_unless(strncmp(buffer_get_data(out),
                      "000fobjid1 foo\n"
                      "000fobjid2 bar\n"
                      "0000", 34) == 0);
  fail_unless(buffer_get_size(err) == 0);
}
END_TEST

START_TEST(fetch_receive_pack) {
  fail_unless(reference_discovery("xxx", process_receive_pack, out, err,
                                  refs_stub) == 0);
  fail_unless(buffer_get_size(out) == 34);
  fail_unless(strncmp(buffer_get_data(out),
                      "000fobjid1 foo\n"
                      "000fobjid2 bar\n"
                      "0000", 34) == 0);
  fail_unless(buffer_get_size(err) == 0);
}
END_TEST

TCase* reference_discovery_tcase() {
  TCase* tc = tcase_create("reference discovery");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, null_out_repository);
  tcase_add_test(tc, null_out_buffer);
  tcase_add_test(tc, null_err_buffer);
  tcase_add_test(tc, null_refs);
  tcase_add_test(tc, empty_upload_pack);
  tcase_add_test(tc, empty_receive_pack);
  tcase_add_test(tc, fetch_failed_upload_pack);
  tcase_add_test(tc, fetch_failed_receive_pack);
  tcase_add_test(tc, fetch_upload_pack);
  tcase_add_test(tc, fetch_receive_pack);

  return tc;
}

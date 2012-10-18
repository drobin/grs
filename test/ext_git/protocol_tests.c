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

#include <stdio.h>

#include "../../src/extension/git/protocol.h"

static int no_refs_stub(const char* repository,
                        struct rd_ref** ref_list, size_t* nrefs) {
  *nrefs = 0;
  return 0;
}

static int failed_stub(const char* repository,
                       struct rd_ref** ref_list, size_t* nrefs) {
  return -1;
}

static buffer_t out;
static buffer_t err;

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

TCase* git_protocol_tcase() {
  TCase* tc = tcase_create("git protocol");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, reference_discovery_null_out_repository);
  tcase_add_test(tc, reference_discovery_null_out_buffer);
  tcase_add_test(tc, reference_discovery_null_err_buffer);
  tcase_add_test(tc, reference_discovery_null_refs);
  tcase_add_test(tc, reference_discovery_empty);
  tcase_add_test(tc, reference_discovery_fetch_failed);

  return tc;
}

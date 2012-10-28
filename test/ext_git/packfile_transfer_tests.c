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

static binbuf_t commits;
static buffer_t out;

static int empty_stub(const char* repository, binbuf_t c, binbuf_t o) {
  fail_unless(strcmp(repository, "xxx") == 0);
  fail_unless(c == commits);
  fail_unless(binbuf_get_size(o) == 0);

  return 0;
}

static int one_stub(const char* repository, binbuf_t c, binbuf_t o) {
  struct packfile_object* obj;

  fail_unless(strcmp(repository, "xxx") == 0);
  fail_unless(c == commits);
  fail_unless(binbuf_get_size(o) == 0);

  obj = binbuf_add(o);
  obj->type = 1;
  obj->content = buffer_create();
  buffer_append(obj->content, "123", 3);

  return 0;
}

static int two_stub(const char* repository, binbuf_t c, binbuf_t o) {
  struct packfile_object* obj;

  fail_unless(strcmp(repository, "xxx") == 0);
  fail_unless(c == commits);
  fail_unless(binbuf_get_size(o) == 0);

  obj = binbuf_add(o);
  obj->type = 1;
  obj->content = buffer_create();
  buffer_append(obj->content, "123", 3);

  obj = binbuf_add(o);
  obj->type = 2;
  obj->content = buffer_create();
  buffer_append(obj->content, "456", 3);

  return 0;
}

static int failed_stub(const char* repository, binbuf_t c, binbuf_t o) {
  fail_unless(strcmp(repository, "xxx") == 0);
  fail_unless(c == commits);
  fail_unless(binbuf_get_size(o) == 0);

  return -1;
}

static void setup() {
  fail_unless((commits = binbuf_create(41)) != NULL);
  fail_unless((out = buffer_create()) != NULL);
}

static void teardown() {
  fail_unless(binbuf_destroy(commits) == 0);
  fail_unless(buffer_destroy(out) == 0);
}

START_TEST(null_repository) {
  fail_unless(packfile_transfer(NULL, commits, empty_stub, out) == -1);
}
END_TEST

START_TEST(null_commits) {
  fail_unless(packfile_transfer("xxx", NULL, empty_stub, out) == -1);
}
END_TEST

START_TEST(null_callback) {
  fail_unless(packfile_transfer("xxx", commits, NULL, out) == -1);
}
END_TEST

START_TEST(null_out) {
  fail_unless(packfile_transfer("xxx", commits, empty_stub, NULL) == -1);
}
END_TEST

START_TEST(failed_callback) {
  fail_unless(packfile_transfer("xxx", commits, failed_stub, out) == -1);
  fail_unless(buffer_get_size(out) == 0);
}
END_TEST

START_TEST(no_objects) {
  unsigned char expected_out[] = {
    0x50, 0x41, 0x43, 0x4b, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x9d, 0x08, 0x82, 0x3b, 0xd8, 0xa8, 0xea,
    0xb5, 0x10, 0xad, 0x6a, 0xc7, 0x5c, 0x82, 0x3c, 0xfd, 0x3e,
    0xd3, 0x1e
  };

  fail_unless(packfile_transfer("xxx", commits, empty_stub, out) == 0);
  fail_unless(buffer_get_size(out) == 32);
  fail_unless(memcmp(buffer_get_data(out), expected_out, 32) == 0);
}
END_TEST

START_TEST(one_object) {
  unsigned char expected_out[] = {
    0x50, 0x41, 0x43, 0x4b, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x00, 0x01, 0x13, 0x78, 0x9c, 0x33, 0x34, 0x32, 0x06, 0x00,
    0x01, 0x2d, 0x00, 0x97, 0x07, 0x77, 0x8d, 0x37, 0xa2, 0x12,
    0x33, 0xad, 0xe4, 0x78, 0xfb, 0x0e, 0xa1, 0x9e, 0x52, 0x35,
    0x10, 0xc3, 0x35, 0x2b
  };

  fail_unless(packfile_transfer("xxx", commits, one_stub, out) == 0);
  fail_unless(buffer_get_size(out) == 44);
  fail_unless(memcmp(buffer_get_data(out), expected_out, 44) == 0);
}
END_TEST

START_TEST(two_objects) {
  unsigned char expected_out[] = {
    0x50, 0x41, 0x43, 0x4b, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x00, 0x02, 0x13, 0x78, 0x9c, 0x33, 0x34, 0x32, 0x06, 0x00,
    0x01, 0x2d, 0x00, 0x97, 0x23, 0x78, 0x9c, 0x33, 0x31, 0x35,
    0x03, 0x00, 0x01, 0x3f, 0x00, 0xa0, 0x9b, 0x5b, 0x1b, 0xe5,
    0xab, 0x9f, 0x00, 0x63, 0x3b, 0xd5, 0xbe, 0x8b, 0x19, 0xa6,
    0x06, 0x28, 0x07, 0xfd, 0x58, 0x0b
  };

  fail_unless(packfile_transfer("xxx", commits, two_stub, out) == 0);
  fail_unless(buffer_get_size(out) == 56);
  fail_unless(memcmp(buffer_get_data(out), expected_out, 56) == 0);
}
END_TEST

TCase* packfile_transfer_tcase() {
  TCase* tc = tcase_create("packfile transfer");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, null_repository);
  tcase_add_test(tc, null_commits);
  tcase_add_test(tc, null_callback);
  tcase_add_test(tc, null_out);
  tcase_add_test(tc, failed_callback);
  tcase_add_test(tc, no_objects);
  tcase_add_test(tc, one_object);
  tcase_add_test(tc, two_objects);

  return tc;
}

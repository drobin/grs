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

#include "../../src/extension/git/checksum.h"

static buffer_t buffer;
static unsigned char sha[20];

static void setup() {
  fail_unless((buffer = buffer_create()) != NULL);
  memset(sha, 0, sizeof(sha));
}

static void teardown() {
  fail_unless(buffer_destroy(buffer) == 0);
  buffer = NULL;
}

START_TEST(checksum_null_buffer) {
  fail_unless(buffer_checksum(NULL, sha) == -1);
}
END_TEST

START_TEST(checksum_null_sha) {
  fail_unless(buffer_checksum(buffer, NULL) == -1);
}
END_TEST

START_TEST(checksum_success) {
  const unsigned char expected_sha[] = {
    0x01, 0xb3, 0x07, 0xac, 0xba, 0x4f, 0x54, 0xf5, 0x5a, 0xaf,
    0xc3, 0x3b, 0xb0, 0x6b, 0xbb, 0xf6, 0xca, 0x80, 0x3e, 0x9a
  };

  fail_unless(buffer_append(buffer, "1234567890", 10) == 0);
  fail_unless(buffer_checksum(buffer, sha) == 0);
  fail_unless(memcmp(sha, expected_sha, 20) == 0);
}
END_TEST

TCase* checksum_tcase() {
  TCase* tc = tcase_create("checksum");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, checksum_null_buffer);
  tcase_add_test(tc, checksum_null_sha);
  tcase_add_test(tc, checksum_success);

  return tc;
}

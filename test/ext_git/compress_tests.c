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

#include "../../src/extension/git/compress.h"

static buffer_t source;
static buffer_t dest;

static void setup() {
  fail_unless((source = buffer_create()) != NULL);
  fail_unless((dest = buffer_create()) != NULL);
}

static void teardown() {
  fail_unless(buffer_destroy(source) == 0);
  fail_unless(buffer_destroy(dest) == 0);
  source = NULL;
  dest = NULL;
}

START_TEST(compress_null_source) {
  fail_unless(buffer_compress(NULL, dest) == -1);
}
END_TEST

START_TEST(compress_null_dest) {
  fail_unless(buffer_compress(source, NULL) == -1);
}
END_TEST

START_TEST(compress_success) {
  const unsigned char expected_dest[] = {
    0x78, 0x9c, 0x33, 0x34, 0x32, 0x36, 0x31, 0x35, 0x33, 0xb7,
    0xb0, 0x34, 0x00, 0x00, 0x0b, 0x2c, 0x02, 0x0e
  };

  fail_unless(buffer_append(source, "1234567890", 10) == 0);
  fail_unless(buffer_compress(source, dest) == 0);
  fail_unless(buffer_get_size(dest) == 18);
  fail_unless(memcmp(buffer_get_data(dest), expected_dest, 8) == 0);
}
END_TEST

TCase* compress_tcase() {
  TCase* tc = tcase_create("compress");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, compress_null_source);
  tcase_add_test(tc, compress_null_dest);
  tcase_add_test(tc, compress_success);

  return tc;
}

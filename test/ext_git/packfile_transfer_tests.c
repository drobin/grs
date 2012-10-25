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

static void setup() {
  fail_unless((commits = binbuf_create(41)) != NULL);
  fail_unless((out = buffer_create()) != NULL);
}

static void teardown() {
  fail_unless(binbuf_destroy(commits) == 0);
  fail_unless(buffer_destroy(out) == 0);
}

START_TEST(null_commits) {
  fail_unless(packfile_transfer(NULL, out) == -1);
}
END_TEST

START_TEST(null_out) {
  fail_unless(packfile_transfer(commits, NULL) == -1);
}
END_TEST

TCase* packfile_transfer_tcase() {
  TCase* tc = tcase_create("packfile transfer");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, null_commits);
  tcase_add_test(tc, null_out);

  return tc;
}

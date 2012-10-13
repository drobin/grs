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

#include "../../src/extension/git/git.h"

grs_t grs;

static void setup() {
  fail_unless((grs = grs_init()) != NULL);
  fail_unless(load_git_extension(grs) == 0);
}

static void teardown() {
  fail_unless(grs_destroy(grs) == 0);
  grs = NULL;
}

static char* git_upload_pack[] = { "git-upload-pack", NULL };

START_TEST(load_extension) {
  fail_unless(grs_get_command(grs, git_upload_pack) != NULL);
}
END_TEST

START_TEST(empty_reference_discovery) {
  command_hook hook;
  buffer_t in, out, err;

  fail_unless((hook = grs_get_command(grs, git_upload_pack)) != NULL);
  fail_unless((in = buffer_create()) != NULL);
  fail_unless((out = buffer_create()) != NULL);
  fail_unless((err = buffer_create()) != NULL);

  fail_unless(hook(git_upload_pack, in, out, err) == 0);
  fail_unless(buffer_get_size(in) == 0);
  fail_unless(buffer_get_size(err) == 0);
  fail_unless(buffer_get_size(out) == 4);
  fail_unless(strncmp(buffer_get_data(out), "0000", 4) == 0);

  fail_unless(buffer_destroy(in) == 0);
  fail_unless(buffer_destroy(out) == 0);
  fail_unless(buffer_destroy(err) == 0);
}
END_TEST

TCase* git_extension_tcase() {
  TCase* tc = tcase_create("git extension");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, load_extension);
  tcase_add_test(tc, empty_reference_discovery);

  return tc;
}

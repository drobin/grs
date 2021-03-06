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

START_TEST(upload_pack_command) {
  char* command[] = { "git-upload-pack", NULL };
  grs_t grs;
  struct command_hooks* hooks;

  fail_unless((grs = grs_init()) != NULL);
  fail_unless(load_git_extension(grs) == 0);

  fail_unless((hooks = grs_get_command_hooks(grs, command)) != NULL);
  fail_unless(hooks->init != NULL);
  fail_unless(hooks->exec != NULL);
  fail_unless(hooks->destroy != NULL);

  fail_unless(grs_destroy(grs) == 0);
}
END_TEST

START_TEST(receive_pack_command) {
  char* command[] = { "git-receive-pack", NULL };
  grs_t grs;
  struct command_hooks* hooks;

  fail_unless((grs = grs_init()) != NULL);
  fail_unless(load_git_extension(grs) == 0);

  fail_unless((hooks = grs_get_command_hooks(grs, command)) != NULL);
  fail_unless(hooks->init != NULL);
  fail_unless(hooks->exec != NULL);
  fail_unless(hooks->destroy != NULL);

  fail_unless(grs_destroy(grs) == 0);
}
END_TEST

TCase* git_extension_tcase() {
  TCase* tc = tcase_create("git extension");

  tcase_add_test(tc, upload_pack_command);
  tcase_add_test(tc, receive_pack_command);

  return tc;
}

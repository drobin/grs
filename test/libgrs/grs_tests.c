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

#include <sys/errno.h>
#include <check.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "../../src/libgrs/grs.h"

static int init_hook_1(char *const command[], void** payload) {
  return 0;
}

static int exec_hook_1(buffer_t in_buf, buffer_t out_buf, buffer_t err_buf,
                       void* payload) {
  return 0;
}

static int exec_hook_2(buffer_t in_buf, buffer_t out_buf, buffer_t err_buf,
                       void* payload) {
  return 0;
}

static void destroy_hook(void* payload) {
}

static grs_t handle;

static void setup() {
  fail_unless((handle = grs_init()) != NULL);
}

static void teardown() {
  fail_unless(grs_destroy(handle) == 0);
  handle = NULL;
}

START_TEST(register_command_null_handle) {
  struct command_hooks hooks;
  hooks.exec = exec_hook_1;
  char* command[] = { "foobar", NULL };

  fail_unless(grs_register_command(NULL, command, &hooks) == -1);
}
END_TEST

START_TEST(register_command_null_command) {
  struct command_hooks hooks;
  hooks.exec = exec_hook_1;

  fail_unless(grs_register_command(handle, NULL, &hooks) == -1);
}
END_TEST

START_TEST(register_command_empty_command) {
  char* command[] = { NULL };
  struct command_hooks hooks;
  hooks.exec = exec_hook_1;

  fail_unless(grs_register_command(handle, command, &hooks) == -1);
}
END_TEST

START_TEST(register_command_null_hooks) {
  char* command[] = { "foobar", NULL };
  fail_unless(grs_register_command(handle, command, NULL) == -1);
}
END_TEST

START_TEST(register_command_null_exec_hook) {
  char* command[] = { "foobar", NULL };
  struct command_hooks hooks;
  hooks.exec = NULL;
  fail_unless(grs_register_command(handle, command, &hooks) == -1);
}
END_TEST

START_TEST(register_command_only_exec) {
  char* command[] = { "foo", NULL };
  struct command_hooks in_hooks;
  struct command_hooks* hooks;

  in_hooks.init = NULL;
  in_hooks.exec = exec_hook_1;
  in_hooks.destroy = NULL;
  fail_unless(grs_register_command(handle, command, &in_hooks) == 0);

  fail_unless((hooks = grs_get_command_hooks(handle, command)) != NULL);
  fail_unless(hooks->init == NULL);
  fail_unless(hooks->exec == exec_hook_1);
  fail_unless(hooks->destroy == NULL);
}
END_TEST

START_TEST(register_command_with_init) {
  char* command[] = { "foo", NULL };
  struct command_hooks in_hooks;
  struct command_hooks* hooks;

  in_hooks.init = init_hook_1;
  in_hooks.exec = exec_hook_1;
  in_hooks.destroy = NULL;
  fail_unless(grs_register_command(handle, command, &in_hooks) == 0);

  fail_unless((hooks = grs_get_command_hooks(handle, command)) != NULL);
  fail_unless(hooks->init == init_hook_1);
  fail_unless(hooks->exec == exec_hook_1);
  fail_unless(hooks->destroy == NULL);
}
END_TEST

START_TEST(register_command_with_destroy) {
  char* command[] = { "foo", NULL };
  struct command_hooks in_hooks;
  struct command_hooks* hooks;

  in_hooks.init = NULL;
  in_hooks.exec = exec_hook_1;
  in_hooks.destroy = destroy_hook;
  fail_unless(grs_register_command(handle, command, &in_hooks) == 0);

  fail_unless((hooks = grs_get_command_hooks(handle, command)) != NULL);
  fail_unless(hooks->init == NULL);
  fail_unless(hooks->exec == exec_hook_1);
  fail_unless(hooks->destroy == destroy_hook);
}
END_TEST

START_TEST(register_command_with_init_destroy) {
  char* command[] = { "foo", NULL };
  struct command_hooks in_hooks;
  struct command_hooks* hooks;

  in_hooks.init = init_hook_1;
  in_hooks.exec = exec_hook_1;
  in_hooks.destroy = destroy_hook;
  fail_unless(grs_register_command(handle, command, &in_hooks) == 0);

  fail_unless((hooks = grs_get_command_hooks(handle, command)) != NULL);
  fail_unless(hooks->init == init_hook_1);
  fail_unless(hooks->exec == exec_hook_1);
  fail_unless(hooks->destroy == destroy_hook);
}
END_TEST

START_TEST(get_command_hooks_null_handle) {
  char* command[] = { "foo", NULL };
  fail_unless(grs_get_command_hooks(NULL, command) == NULL);
}
END_TEST

START_TEST(get_command_hooks_null_command) {
  fail_unless(grs_get_command_hooks(handle, NULL) == NULL);
}
END_TEST

START_TEST(get_command_hooks_empty_command) {
  char* command[] = { NULL };
  fail_unless(grs_get_command_hooks(handle, command) == NULL);
}
END_TEST

START_TEST(get_command_hooks_found) {
  char* command_1[] = { "hook1", NULL };
  char* command_2[] = { "hook2", NULL };
  struct command_hooks in_hooks_1, in_hooks_2;
  struct command_hooks* hooks_1;
  struct command_hooks* hooks_2;

  in_hooks_1.exec = exec_hook_1;
  in_hooks_2.exec = exec_hook_2;
  fail_unless(grs_register_command(handle, command_1, &in_hooks_1) == 0);
  fail_unless(grs_register_command(handle, command_2, &in_hooks_2) == 0);

  fail_unless((hooks_1 = grs_get_command_hooks(handle, command_1)) != NULL);
  fail_unless((hooks_2 = grs_get_command_hooks(handle, command_2)) != NULL);
  fail_unless(hooks_1->exec == exec_hook_1);
  fail_unless(hooks_2->exec == exec_hook_2);
}
END_TEST

START_TEST(get_command_hooks_hierarchy) {
  char* part[] = { "foo", NULL };
  char* leaf[] = { "foo", "bar", NULL };
  struct command_hooks in_hooks;
  struct command_hooks* hooks;

  in_hooks.exec = exec_hook_1;
  fail_unless(grs_register_command(handle, leaf, &in_hooks) == 0);

  fail_unless(grs_get_command_hooks(handle, part) == NULL);
  fail_unless((hooks = grs_get_command_hooks(handle, leaf)) != NULL);
  fail_unless(hooks->exec == exec_hook_1);
}
END_TEST

START_TEST(get_command_hooks_partly_hierarchy) {
  char* part[] = { "foo", NULL };
  char* leaf[] = { "foo", "bar", NULL };
  struct command_hooks in_hooks;
  struct command_hooks* hooks;

  in_hooks.exec = exec_hook_1;
  fail_unless(grs_register_command(handle, part, &in_hooks) == 0);

  fail_unless((hooks = grs_get_command_hooks(handle, part)) != NULL);
  fail_unless(hooks->exec == exec_hook_1);
  fail_unless((hooks = grs_get_command_hooks(handle, leaf)) != NULL);
  fail_unless(hooks->exec == exec_hook_1);
}
END_TEST

START_TEST(get_command_hooks_not_found) {
  char* command_1[] = { "hook1", NULL };
  char* command_2[] = { "hook2", NULL };
  char* command_3[] = { "hook3", NULL };
  struct command_hooks in_hooks_1, in_hooks_2;

  in_hooks_1.exec = exec_hook_1;
  in_hooks_2.exec = exec_hook_2;
  fail_unless(grs_register_command(handle, command_1, &in_hooks_1) == 0);
  fail_unless(grs_register_command(handle, command_2, &in_hooks_2) == 0);
  fail_unless(grs_get_command_hooks(handle, command_3) == NULL);
}
END_TEST

START_TEST(get_command_hooks_not_found_hierarchy) {
  char* command[] = { "1", "2", "3", NULL };
  fail_unless(grs_get_command_hooks(handle, command) == NULL);
}
END_TEST

START_TEST(destroy_null_handle) {
  fail_unless(grs_destroy(NULL) == -1);
}
END_TEST

START_TEST(get_acl_null_handle) {
  fail_unless(grs_get_acl(NULL) == NULL);
}
END_TEST

START_TEST(get_acl) {
  fail_unless(grs_get_acl(handle) != NULL);
}
END_TEST

TCase* grs_tcase() {
  TCase* tc = tcase_create("grs");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, destroy_null_handle);
  tcase_add_test(tc, get_acl_null_handle);
  tcase_add_test(tc, get_acl);
  tcase_add_test(tc, register_command_null_handle);
  tcase_add_test(tc, register_command_null_command);
  tcase_add_test(tc, register_command_empty_command);
  tcase_add_test(tc, register_command_null_hooks);
  tcase_add_test(tc, register_command_null_exec_hook);
  tcase_add_test(tc, register_command_only_exec);
  tcase_add_test(tc, register_command_with_init);
  tcase_add_test(tc, register_command_with_destroy);
  tcase_add_test(tc, register_command_with_init_destroy);
  tcase_add_test(tc, get_command_hooks_null_handle);
  tcase_add_test(tc, get_command_hooks_null_command);
  tcase_add_test(tc, get_command_hooks_empty_command);
  tcase_add_test(tc, get_command_hooks_found);
  tcase_add_test(tc, get_command_hooks_hierarchy);
  tcase_add_test(tc, get_command_hooks_partly_hierarchy);
  tcase_add_test(tc, get_command_hooks_not_found);
  tcase_add_test(tc, get_command_hooks_not_found_hierarchy);

  return tc;
}

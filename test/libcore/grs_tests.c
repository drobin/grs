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

#include "../../src/libgrs/core/grs.h"

static int sample_command_hook_1(const char** command, buffer_t in_buf, buffer_t out_buf) { return 0; }
static int sample_command_hook_2(const char** command, buffer_t in_buf, buffer_t out_buf) { return 0; }

static grs_t handle;

static void setup() {
  fail_unless((handle = grs_init()) != NULL);
}

static void teardown() {
  fail_unless(grs_destroy(handle) == 0);
  handle = NULL;
}

START_TEST(register_command_null_env) {
  command_hook hook;
  fail_unless(grs_register_command(NULL, "foobar", hook) == -1);
}
END_TEST

START_TEST(register_command_null_command) {
  command_hook hook;
  fail_unless(grs_register_command(handle, NULL, hook) == -1);
}
END_TEST

START_TEST(register_command_null_hook) {
  fail_unless(grs_register_command(handle, "foobar", NULL) == -1);
}
END_TEST

START_TEST(get_command_null_env) {
  fail_unless(grs_get_command(NULL, "foobar") == NULL);
}
END_TEST

START_TEST(get_command_null_command) {
  fail_unless(grs_get_command(handle, NULL) == NULL);
}
END_TEST

START_TEST(get_command_found) {
  command_hook h1 = sample_command_hook_1;
  command_hook h2 = sample_command_hook_2;

  fail_unless(grs_register_command(handle, "hook1", h1) == 0);
  fail_unless(grs_register_command(handle, "hook2", h2) == 0);
  fail_unless(grs_get_command(handle, "hook1") == h1);
  fail_unless(grs_get_command(handle, "hook2") == h2);
}
END_TEST


START_TEST(get_command_not_found) {
  command_hook h1 = sample_command_hook_1;
  command_hook h2 = sample_command_hook_2;

  fail_unless(grs_register_command(handle, "hook1", h1) == 0);
  fail_unless(grs_register_command(handle, "hook2", h2) == 0);
  fail_unless(grs_get_command(handle, "hook3") == NULL);
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
  tcase_add_test(tc, register_command_null_env);
  tcase_add_test(tc, register_command_null_command);
  tcase_add_test(tc, register_command_null_hook);
  tcase_add_test(tc, get_command_null_env);
  tcase_add_test(tc, get_command_null_command);
  tcase_add_test(tc, get_command_found);
  tcase_add_test(tc, get_command_not_found);

  return tc;
}

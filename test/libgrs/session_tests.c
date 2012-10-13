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

#include "../../src/libgrs/session.h"

int success_hook(char *const command[], buffer_t in_buf, buffer_t out_buf,
                 buffer_t err_buf) {
  return 0;
}

int fail_hook(char *const command[], buffer_t in_buf, buffer_t out_buf,
              buffer_t err_buf) {
  return -1;
}

int continue_hook(char *const command[], buffer_t in_buf, buffer_t out_buf,
                  buffer_t err_buf) {
  return 1;
}

static grs_t grs;
static session_t session;

static void setup() {
  acl_t acl;
  acl_node_t node;
  struct acl_node_value* value;

  fail_unless((grs = grs_init()) != NULL);
  fail_unless((session = session_create(grs)) != NULL);

  // Setup ACL-system, that everybody can do everything
  fail_unless((acl = grs_get_acl(session_get_grs(session))) != NULL);
  fail_unless((node = acl_get_root_node(acl)) != NULL);
  fail_unless((value = acl_node_get_value(node, 1)) != NULL);
  value->flag = 1;
}

static void teardown() {
  fail_unless(session_destroy(session) == 0);
  fail_unless(grs_destroy(grs) == 0);
  session = NULL;
  grs = NULL;
}

START_TEST(create_null_grs) {
  fail_unless(session_create(NULL) == NULL);
}
END_TEST

START_TEST(destroy_null_session) {
  fail_unless(session_destroy(NULL) == -1);
}
END_TEST

START_TEST(get_grs_null_session) {
  fail_unless(session_get_grs(NULL) == NULL);
}
END_TEST

START_TEST(get_grs) {
  fail_unless(session_get_grs(session) == grs);
}
END_TEST

START_TEST(authenticate_null_session) {
  fail_unless(session_authenticate(NULL, "foo", "foo") == -1);
}
END_TEST

START_TEST(authenticate_null_username) {
  fail_unless(session_authenticate(session, NULL, "foo") == -1);
}
END_TEST

START_TEST(authenticate_null_password) {
  fail_unless(session_authenticate(session, "foo", NULL) == -1);
}
END_TEST

START_TEST(authenticate_wrong_password) {
  fail_unless(session_authenticate(session, "foo", "bar") == -1);
}
END_TEST

START_TEST(authenticate_success) {
  fail_unless(session_authenticate(session, "foo", "foo") == 0);
}
END_TEST

START_TEST(get_command_null_session) {
  fail_unless(session_get_command(NULL) == NULL);
}
END_TEST

START_TEST(get_command_initial) {
  fail_unless(session_get_command(session)[0] == NULL);
}
END_TEST

START_TEST(get_command_one_arg) {
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(strcmp(session_get_command(session)[0], "foo") == 0);
  fail_unless(session_get_command(session)[1] == NULL);
}
END_TEST

START_TEST(get_command_two_args) {
  fail_unless(session_set_command(session, "foo bar") == 0);
  fail_unless(strcmp(session_get_command(session)[0], "foo") == 0);
  fail_unless(strcmp(session_get_command(session)[1], "bar") == 0);
  fail_unless(session_get_command(session)[2] == NULL);
}
END_TEST

START_TEST(set_command_null_session) {
  fail_unless(session_set_command(NULL, "foo") == -1);
}
END_TEST

START_TEST(set_command_null_command) {
  fail_unless(session_set_command(session, NULL) == -1);
}
END_TEST

START_TEST(get_in_buffer_null_session) {
  fail_unless(session_get_in_buffer(NULL) == NULL);
}
END_TEST

START_TEST(get_in_buffer) {
  fail_unless(session_get_in_buffer(session) != NULL);
}
END_TEST

START_TEST(get_out_buffer_null_session) {
  fail_unless(session_get_out_buffer(NULL) == NULL);
}
END_TEST

START_TEST(get_out_buffer) {
  fail_unless(session_get_out_buffer(session) != NULL);
}
END_TEST

START_TEST(get_err_buffer_null_session) {
  fail_unless(session_get_err_buffer(NULL) == NULL);
}
END_TEST

START_TEST(get_err_buffer) {
  fail_unless(session_get_err_buffer(session) != NULL);
}
END_TEST

START_TEST(can_exec_null_session) {
  fail_unless(session_can_exec(NULL) == -1);
}
END_TEST

START_TEST(can_exec_no_command) {
  fail_unless(!session_can_exec(session));
}
END_TEST

START_TEST(can_exec_with_command) {
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(session_can_exec(session));
}
END_TEST

START_TEST(exec_null_session) {
  fail_unless(session_exec(NULL) == -1);
}
END_TEST

START_TEST(exec_no_hook) {
  fail_unless(session_exec(session) == -1);
}
END_TEST

START_TEST(exec_success) {
  char* command[] = { "foo", NULL };
  grs_t grs = session_get_grs(session);

  fail_unless(grs_register_command(grs, command, success_hook) == 0);
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(session_exec(session) == 0);
}
END_TEST

START_TEST(is_finished_null_session) {
  fail_unless(session_is_finished(NULL) == -1);
}
END_TEST

START_TEST(is_finished_no_exec) {
  fail_unless(!session_is_finished(session));
}
END_TEST

START_TEST(is_finished_error) {
  char* command[] = { "foo", NULL };
  grs_t grs = session_get_grs(session);

  fail_unless(grs_register_command(grs, command, fail_hook) == 0);
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(session_exec(session) == -1);
  fail_unless(session_is_finished(session));
}
END_TEST

START_TEST(is_finished_continue) {
  char* command[] = { "foo", NULL };
  grs_t grs = session_get_grs(session);

  fail_unless(grs_register_command(grs, command, continue_hook) == 0);
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(session_exec(session) == 1);
  fail_unless(!session_is_finished(session));
}
END_TEST

START_TEST(is_finished_success) {
  char* command[] = { "foo", NULL };
  grs_t grs = session_get_grs(session);

  fail_unless(grs_register_command(grs, command, success_hook) == 0);
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(session_exec(session) == 0);
  fail_unless(session_is_finished(session));
}
END_TEST

TCase* session_tcase() {
  TCase* tc = tcase_create("session");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, create_null_grs);
  tcase_add_test(tc, destroy_null_session);
  tcase_add_test(tc, get_grs_null_session);
  tcase_add_test(tc, get_grs);
  tcase_add_test(tc, authenticate_null_session);
  tcase_add_test(tc, authenticate_null_username);
  tcase_add_test(tc, authenticate_null_password);
  tcase_add_test(tc, authenticate_wrong_password);
  tcase_add_test(tc, authenticate_success);
  tcase_add_test(tc, get_command_null_session);
  tcase_add_test(tc, get_command_initial);
  tcase_add_test(tc, get_command_one_arg);
  tcase_add_test(tc, get_command_two_args);
  tcase_add_test(tc, set_command_null_session);
  tcase_add_test(tc, set_command_null_command);
  tcase_add_test(tc, get_in_buffer_null_session);
  tcase_add_test(tc, get_in_buffer);
  tcase_add_test(tc, get_out_buffer_null_session);
  tcase_add_test(tc, get_out_buffer);
  tcase_add_test(tc, get_err_buffer_null_session);
  tcase_add_test(tc, get_err_buffer);
  tcase_add_test(tc, can_exec_null_session);
  tcase_add_test(tc, can_exec_no_command);
  tcase_add_test(tc, can_exec_with_command);
  tcase_add_test(tc, exec_null_session);
  tcase_add_test(tc, exec_no_hook);
  tcase_add_test(tc, exec_success);
  tcase_add_test(tc, is_finished_null_session);
  tcase_add_test(tc, is_finished_no_exec);
  tcase_add_test(tc, is_finished_error);
  tcase_add_test(tc, is_finished_continue);
  tcase_add_test(tc, is_finished_success);

  return tc;
}

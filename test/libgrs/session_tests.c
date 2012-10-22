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

static int n_init = 0;
static int n_exec = 0;
static int n_destroy = 0;

struct payload_data {
} data;

int init_hook(char *const command[], void** payload) {
  *payload = &data;
  n_init++;
  return 0;
}

int init_failed_hook(char *const command[], void** payload) {
  n_init++;
  return -1;
}

int exec_hook(char *const command[], buffer_t in_buf, buffer_t out_buf,
              buffer_t err_buf) {
  n_exec++;
  return 0;
}

int exec_failed_hook(char *const command[], buffer_t in_buf, buffer_t out_buf,
                     buffer_t err_buf) {
  n_exec++;
  return -1;
}

int exec_continue_hook(char *const command[], buffer_t in_buf, buffer_t out_buf,
                       buffer_t err_buf) {
  n_exec++;
  return 1;
}

void destroy_hook(void* payload) {
  n_destroy++;
}

void destroy_payload_hook(void* payload) {
  n_destroy++;
  fail_unless(payload == &data);
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

  n_init = 0;
  n_exec = 0;
  n_destroy = 0;
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

START_TEST(exec_init_failed) {
  char* command[] = { "foo", NULL };
  grs_t grs = session_get_grs(session);
  struct command_hooks hooks;

  hooks.init = init_failed_hook;
  hooks.exec = exec_hook;
  hooks.destroy = destroy_hook;
  fail_unless(grs_register_command(grs, command, &hooks) == 0);
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(session_exec(session) == -1);
  fail_unless(session_is_finished(session));
  fail_unless(n_init == 1);
  fail_unless(n_exec == 0);
  fail_unless(n_destroy == 0);
}
END_TEST

START_TEST(exec_init_skipped_exec_failed_destroy_skipped) {
  char* command[] = { "foo", NULL };
  grs_t grs = session_get_grs(session);
  struct command_hooks hooks;

  hooks.init = NULL;
  hooks.exec = exec_failed_hook;
  hooks.destroy = NULL;
  fail_unless(grs_register_command(grs, command, &hooks) == 0);
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(session_exec(session) == -1);
  fail_unless(session_is_finished(session));
  fail_unless(n_init == 0);
  fail_unless(n_exec == 1);
  fail_unless(n_destroy == 0);
}
END_TEST

START_TEST(exec_init_skipped_exec_continue) {
  char* command[] = { "foo", NULL };
  grs_t grs = session_get_grs(session);
  struct command_hooks hooks;

  hooks.init = NULL;
  hooks.exec = exec_continue_hook;
  hooks.destroy = destroy_hook;
  fail_unless(grs_register_command(grs, command, &hooks) == 0);
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(session_exec(session) == 1);
  fail_unless(!session_is_finished(session));
  fail_unless(n_init == 0);
  fail_unless(n_exec == 1);
  fail_unless(n_destroy == 0);
}
END_TEST

START_TEST(exec_init_skipped_exec_continue_success) {
  char* command[] = { "foo", NULL };
  grs_t grs = session_get_grs(session);
  struct command_hooks hooks;

  hooks.init = NULL;
  hooks.exec = exec_continue_hook;
  hooks.destroy = destroy_hook;
  fail_unless(grs_register_command(grs, command, &hooks) == 0);
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(session_exec(session) == 1);
  fail_unless(!session_is_finished(session));

  hooks.exec = exec_hook;
  fail_unless(grs_register_command(grs, command, &hooks) == 0);
  fail_unless(session_exec(session) == 0);
  fail_unless(session_is_finished(session));

  fail_unless(n_init == 0);
  fail_unless(n_exec == 2);
  fail_unless(n_destroy == 1);
}
END_TEST

START_TEST(exec_init_skipped_exec_destroy_skipped) {
  char* command[] = { "foo", NULL };
  grs_t grs = session_get_grs(session);
  struct command_hooks hooks;

  hooks.init = NULL;
  hooks.exec = exec_hook;
  hooks.destroy = NULL;
  fail_unless(grs_register_command(grs, command, &hooks) == 0);
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(session_exec(session) == 0);
  fail_unless(session_is_finished(session));
  fail_unless(n_init == 0);
  fail_unless(n_exec == 1);
  fail_unless(n_destroy == 0);
}
END_TEST

START_TEST(exec_init_skipped_exec_destroy) {
  char* command[] = { "foo", NULL };
  grs_t grs = session_get_grs(session);
  struct command_hooks hooks;

  hooks.init = NULL;
  hooks.exec = exec_hook;
  hooks.destroy = destroy_hook;
  fail_unless(grs_register_command(grs, command, &hooks) == 0);
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(session_exec(session) == 0);
  fail_unless(session_is_finished(session));
  fail_unless(n_init == 0);
  fail_unless(n_exec == 1);
  fail_unless(n_destroy == 1);
}
END_TEST

START_TEST(exec_init_exec_failed_destroy_skipped) {
  char* command[] = { "foo", NULL };
  grs_t grs = session_get_grs(session);
  struct command_hooks hooks;

  hooks.init = init_hook;
  hooks.exec = exec_failed_hook;
  hooks.destroy = NULL;
  fail_unless(grs_register_command(grs, command, &hooks) == 0);
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(session_exec(session) == -1);
  fail_unless(session_is_finished(session));
  fail_unless(n_init == 1);
  fail_unless(n_exec == 1);
  fail_unless(n_destroy == 0);
}
END_TEST

START_TEST(exec_init_exec_continue) {
  char* command[] = { "foo", NULL };
  grs_t grs = session_get_grs(session);
  struct command_hooks hooks;

  hooks.init = init_hook;
  hooks.exec = exec_continue_hook;
  hooks.destroy = destroy_hook;
  fail_unless(grs_register_command(grs, command, &hooks) == 0);
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(session_exec(session) == 1);
  fail_unless(!session_is_finished(session));
  fail_unless(n_init == 1);
  fail_unless(n_exec == 1);
  fail_unless(n_destroy == 0);
}
END_TEST

START_TEST(exec_init_exec_continue_success) {
  char* command[] = { "foo", NULL };
  grs_t grs = session_get_grs(session);
  struct command_hooks hooks;

  hooks.init = init_hook;
  hooks.exec = exec_continue_hook;
  hooks.destroy = destroy_hook;
  fail_unless(grs_register_command(grs, command, &hooks) == 0);
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(session_exec(session) == 1);
  fail_unless(!session_is_finished(session));

  hooks.exec = exec_hook;
  fail_unless(grs_register_command(grs, command, &hooks) == 0);
  fail_unless(session_exec(session) == 0);
  fail_unless(session_is_finished(session));

  fail_unless(n_init == 1);
  fail_unless(n_exec == 2);
  fail_unless(n_destroy == 1);
}
END_TEST

START_TEST(exec_init_exec_destroy_skipped) {
  char* command[] = { "foo", NULL };
  grs_t grs = session_get_grs(session);
  struct command_hooks hooks;

  hooks.init = init_hook;
  hooks.exec = exec_hook;
  hooks.destroy = NULL;
  fail_unless(grs_register_command(grs, command, &hooks) == 0);
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(session_exec(session) == 0);
  fail_unless(session_is_finished(session));
  fail_unless(n_init == 1);
  fail_unless(n_exec == 1);
  fail_unless(n_destroy == 0);
}
END_TEST

START_TEST(exec_init_exec_destroy) {
  char* command[] = { "foo", NULL };
  grs_t grs = session_get_grs(session);
  struct command_hooks hooks;

  hooks.init = init_hook;
  hooks.exec = exec_hook;
  hooks.destroy = destroy_hook;
  fail_unless(grs_register_command(grs, command, &hooks) == 0);
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(session_exec(session) == 0);
  fail_unless(session_is_finished(session));
  fail_unless(n_init == 1);
  fail_unless(n_exec == 1);
  fail_unless(n_destroy == 1);
}
END_TEST

START_TEST(exec_payload_check) {
  char* command[] = { "foo", NULL };
  grs_t grs = session_get_grs(session);
  struct command_hooks hooks;

  hooks.init = init_hook;
  hooks.exec = exec_hook;
  hooks.destroy = destroy_payload_hook;
  fail_unless(grs_register_command(grs, command, &hooks) == 0);
  fail_unless(session_set_command(session, "foo") == 0);
  fail_unless(session_exec(session) == 0);
  fail_unless(session_is_finished(session));
  fail_unless(n_init == 1);
  fail_unless(n_exec == 1);
  fail_unless(n_destroy == 1);
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
  tcase_add_test(tc, exec_init_failed);
  tcase_add_test(tc, exec_init_skipped_exec_failed_destroy_skipped);
  tcase_add_test(tc, exec_init_skipped_exec_continue);
  tcase_add_test(tc, exec_init_skipped_exec_continue_success);
  tcase_add_test(tc, exec_init_skipped_exec_destroy_skipped);
  tcase_add_test(tc, exec_init_skipped_exec_destroy);
  tcase_add_test(tc, exec_init_exec_failed_destroy_skipped);
  tcase_add_test(tc, exec_init_exec_continue);
  tcase_add_test(tc, exec_init_exec_continue_success);
  tcase_add_test(tc, exec_init_exec_destroy_skipped);
  tcase_add_test(tc, exec_init_exec_destroy);
  tcase_add_test(tc, exec_payload_check);
  tcase_add_test(tc, is_finished_null_session);
  tcase_add_test(tc, is_finished_no_exec);

  return tc;
}

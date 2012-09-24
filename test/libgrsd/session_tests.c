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

#include "../../src/libgrsd/session.h"

static grs_t grs;
static session_t session;

static void setup() {
  fail_unless((grs = grs_init()) != NULL);
  fail_unless((session = session_create(grs)) != NULL);
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

START_TEST(get_state_null_session) {
  fail_unless(session_get_state(NULL) == -1);
}
END_TEST

START_TEST(get_state_initial) {
  fail_unless(session_get_state(session) == NEED_AUTHENTICATION);
}
END_TEST

START_TEST(set_state_null_session) {
  fail_unless(session_set_state(NULL, EXECUTING) == -1);
}
END_TEST

START_TEST(get_set_state) {
  fail_unless(session_get_state(session) == NEED_AUTHENTICATION);
  fail_unless(session_set_state(session, EXECUTING) == 0);
  fail_unless(session_get_state(session) == EXECUTING);
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

START_TEST(authenticate_wrong_state) {
  fail_unless(session_set_state(session, EXECUTING) == 0);
  fail_unless(session_authenticate(session, "foo", "foo") == -1);
}
END_TEST

START_TEST(authenticate_wrong_password) {
  fail_unless(session_authenticate(session, "foo", "bar") == -1);
  fail_unless(session_get_state(session) == NEED_AUTHENTICATION);
}
END_TEST

START_TEST(authenticate_success) {
  fail_unless(session_authenticate(session, "foo", "foo") == 0);
  fail_unless(session_get_state(session) == NEED_PROCESS);
}
END_TEST

START_TEST(get_process_null_session) {
  fail_unless(session_get_process(NULL) == NULL);
}
END_TEST

START_TEST(create_process_null_session) {
  process_env_t env;

  fail_unless((env = process_env_create()) != NULL);
  fail_unless(session_create_process(NULL, env, "foobar") == NULL);
  fail_unless(session_get_process(session) == NULL);
  fail_unless(process_env_destroy(env) == 0);
}
END_TEST

START_TEST(create_process_null_env) {
  fail_unless(session_create_process(session, NULL, "foobar") == NULL);
  fail_unless(session_get_process(session) == NULL);
}
END_TEST

START_TEST(create_process_null_command) {
  process_env_t env;

  fail_unless((env = process_env_create()) != NULL);
  fail_unless(session_create_process(session, env, NULL) == NULL);
  fail_unless(session_get_process(session) == NULL);
  fail_unless(process_env_destroy(env) == 0);
}
END_TEST

START_TEST(create_process_wrong_state) {
  process_env_t env;

  fail_unless((env = process_env_create()) != NULL);
  fail_unless(session_create_process(session, env, "foobar") == NULL);
  fail_unless(session_get_process(session) == NULL);
  fail_unless(process_env_destroy(env) == 0);
}
END_TEST

START_TEST(create_process_success) {
  process_env_t env;
  process_t process;

  fail_unless((env = process_env_create()) != NULL);
  fail_unless(session_set_state(session, NEED_PROCESS) == 0);
  fail_unless((process = session_create_process(session, env, "foobar")) != NULL);
  fail_unless(session_get_process(session) == process);
  fail_unless(session_get_state(session) == NEED_EXEC);
  fail_unless(process_env_destroy(env) == 0);
}
END_TEST

START_TEST(exec_null_session) {
  fail_unless(session_exec(NULL) == -1);
}
END_TEST

START_TEST(exec_wrong_state) {
  fail_unless(session_exec(session) == -1);
}
END_TEST

START_TEST(exec_success) {
  fail_unless(session_set_state(session, NEED_EXEC) == 0);
  fail_unless(session_exec(session) == 0);
  fail_unless(session_get_state(session) == EXECUTING);
}
END_TEST

TCase* session_tcase() {
  TCase* tc = tcase_create("session");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, create_null_grs);
  tcase_add_test(tc, destroy_null_session);
  tcase_add_test(tc, get_state_null_session);
  tcase_add_test(tc, get_state_initial);
  tcase_add_test(tc, set_state_null_session);
  tcase_add_test(tc, get_set_state);
  tcase_add_test(tc, authenticate_null_session);
  tcase_add_test(tc, authenticate_null_username);
  tcase_add_test(tc, authenticate_null_password);
  tcase_add_test(tc, authenticate_wrong_state);
  tcase_add_test(tc, authenticate_wrong_password);
  tcase_add_test(tc, authenticate_success);
  tcase_add_test(tc, get_process_null_session);
  tcase_add_test(tc, create_process_null_session);
  tcase_add_test(tc, create_process_null_env);
  tcase_add_test(tc, create_process_null_command);
  tcase_add_test(tc, create_process_wrong_state);
  tcase_add_test(tc, create_process_success);
  tcase_add_test(tc, exec_null_session);
  tcase_add_test(tc, exec_wrong_state);
  tcase_add_test(tc, exec_success);

  return tc;
}

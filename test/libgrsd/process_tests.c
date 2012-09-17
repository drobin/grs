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

#include <sys/types.h>
#include <sys/uio.h>
#include <check.h>
#include <stdio.h>
#include <unistd.h>

#include "../../src/libgrsd/process.h"

static int sample_command_hook_1(process_t process) { return 0; }
static int sample_command_hook_2(process_t process) { return 0; }

static int ls_hook(process_t process) {
  return process_fork(process);
}

static process_env_t env;

static void setup() {
  fail_unless((env = process_env_create()) != NULL);
}

static void teardown() {
  fail_unless(process_env_destroy(env) == 0);
}

START_TEST(env_register_command_null_env) {
  command_hook hook;
  fail_unless(process_env_register_command(NULL, "foobar", hook) == -1);
}
END_TEST

START_TEST(env_register_command_null_command) {
  command_hook hook;
  fail_unless(process_env_register_command(env, NULL, hook) == -1);
}
END_TEST

START_TEST(env_register_command_null_hook) {
  fail_unless(process_env_register_command(env, "foobar", NULL) == -1);
}
END_TEST

START_TEST(env_get_command_null_env) {
  fail_unless(process_env_get_command(NULL, "foobar") == NULL);
}
END_TEST

START_TEST(env_get_command_null_command) {
  fail_unless(process_env_get_command(env, NULL) == NULL);
}
END_TEST

START_TEST(env_get_command_found) {
  command_hook h1 = sample_command_hook_1;
  command_hook h2 = sample_command_hook_2;

  fail_unless(process_env_register_command(env, "hook1", h1) == 0);
  fail_unless(process_env_register_command(env, "hook2", h2) == 0);
  fail_unless(process_env_get_command(env, "hook1") == h1);
  fail_unless(process_env_get_command(env, "hook2") == h2);
}
END_TEST


START_TEST(env_get_command_not_found) {
  command_hook h1 = sample_command_hook_1;
  command_hook h2 = sample_command_hook_2;

  fail_unless(process_env_register_command(env, "hook1", h1) == 0);
  fail_unless(process_env_register_command(env, "hook2", h2) == 0);
  fail_unless(process_env_get_command(env, "hook3") == NULL);
}
END_TEST

START_TEST(destroy_null_env) {
  fail_unless(process_env_destroy(NULL) == -1);
}
END_TEST

START_TEST(prepare_null_env) {
  fail_unless(process_prepare(NULL, "foo") == NULL);
}
END_TEST

START_TEST(prepare_null_command) {
  fail_unless(process_prepare(env, NULL) == NULL);
}
END_TEST

START_TEST(prepare_no_args) {
  process_t process;

  fail_unless((process = process_prepare(env, "foobar")) != NULL);
  fail_unless(strcmp(process_get_command(process), "foobar") == 0);
  fail_unless(process_get_args(process)[0] == NULL);
  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(prepare_one_arg) {
  process_t process;

  fail_unless((process = process_prepare(env, "foobar foo")) != NULL);
  fail_unless(strcmp(process_get_command(process), "foobar") == 0);
  fail_unless(strcmp(process_get_args(process)[0], "foo") == 0);
  fail_unless(process_get_args(process)[1] == NULL);
  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(prepare_two_args) {
  process_t process;

  fail_unless((process = process_prepare(env, "foobar foo bar")) != NULL);
  fail_unless(strcmp(process_get_command(process), "foobar") == 0);
  fail_unless(strcmp(process_get_args(process)[0], "foo") == 0);
  fail_unless(strcmp(process_get_args(process)[1], "bar") == 0);
  fail_unless(process_get_args(process)[2] == NULL);
  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(process_destroy_null_process) {
  fail_unless(process_destroy(NULL) == -1);
}
END_TEST

START_TEST(get_env_success) {
  process_t process;

  fail_unless((process = process_prepare(env, "foobar")) != NULL);
  fail_unless(process_get_env(process) == env);
  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(get_env_null_process) {
  fail_unless(process_get_env(NULL) == NULL);
}
END_TEST

START_TEST(get_command_null_process) {
  fail_unless(process_get_command(NULL) == NULL);
}
END_TEST

START_TEST(get_args_null_process) {
  fail_unless(process_get_args(NULL) == NULL);
}
END_TEST

START_TEST(get_fd_in_null_process) {
  fail_unless(process_get_fd_in(NULL) == -1);
}
END_TEST

START_TEST(get_fd_in) {
  process_t process;

  fail_unless((process = process_prepare(env, "foobar")) != NULL);
  fail_unless(process_get_fd_in(process) > 0);
  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(get_fd_out_null_process) {
  fail_unless(process_get_fd_out(NULL) == -1);
}
END_TEST

START_TEST(get_fd_out) {
  process_t process;

  fail_unless((process = process_prepare(env, "foobar")) != NULL);
  fail_unless(process_get_fd_out(process) > 0);
  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(fork_null_process) {
  fail_unless(process_fork(NULL) == -1);
}
END_TEST

START_TEST(fork_absolute_path) {
  process_t process;
  char buf[64];
  size_t nread;

  fail_unless((process = process_prepare(env, "/bin/ls -1")) != NULL);
  fail_unless(process_fork(process) == 0);

  while (!process_is_exited(process));
  fail_unless(process_get_exit_status(process) == 0);

  while ((nread = read(process_get_fd_out(process), buf, sizeof(buf))) != 0) {
    fail_unless(nread > 0);
  }

  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(fork_relative_path) {
  process_t process;
  char buf[64];
  size_t nread;

  fail_unless((process = process_prepare(env, "ls -1")) != NULL);
  fail_unless(process_fork(process) == 0);

  while (!process_is_exited(process));
  fail_unless(process_get_exit_status(process) == 0);

  while ((nread = read(process_get_fd_out(process), buf, sizeof(buf))) != 0) {
    fail_unless(nread > 0);
  }

  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(fork_no_such_file) {
  process_t process;
  char buf[64];
  size_t nread;

  fail_unless((process = process_prepare(env, "foobar")) != NULL);
  fail_unless(process_fork(process) == 0);

  while (!process_is_exited(process));
  fail_unless(process_get_exit_status(process) == 127);

  while ((nread = read(process_get_fd_out(process), buf, sizeof(buf))) != 0) {
    fail_unless(nread > 0);
  }

  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(exec_test_command) {
  process_t process;
  char buf[64];

  fail_unless((process = process_prepare(env, "hello")) != NULL);
  fail_unless(process_exec(process) == 0);

  while (!process_is_exited(process));
  fail_unless(process_get_exit_status(process) == 0);

  fail_unless(read(process_get_fd_out(process), buf, sizeof(buf)) == 12);
  fail_unless(strncmp(buf, "Hello world!", 12) == 0);

  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(exec_null_process) {
  fail_unless(process_exec(NULL) == -1);
}
END_TEST

START_TEST(exec_absolute_path) {
  process_t process;
  char buf[64];
  size_t nread;

  fail_unless(process_env_register_command(env, "/bin/ls", ls_hook) == 0);
  fail_unless((process = process_prepare(env, "/bin/ls -1")) != NULL);
  fail_unless(process_exec(process) == 0);

  while (!process_is_exited(process));
  fail_unless(process_get_exit_status(process) == 0);

  while ((nread = read(process_get_fd_out(process), buf, sizeof(buf))) != 0) {
    fail_unless(nread > 0);
  }

  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(exec_relative_path) {
  process_t process;
  char buf[64];
  size_t nread;

  fail_unless(process_env_register_command(env, "ls", ls_hook) == 0);
  fail_unless((process = process_prepare(env, "ls -1")) != NULL);
  fail_unless(process_exec(process) == 0);

  while (!process_is_exited(process));
  fail_unless(process_get_exit_status(process) == 0);

  while ((nread = read(process_get_fd_out(process), buf, sizeof(buf))) != 0) {
    fail_unless(nread > 0);
  }

  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(exec_no_hook) {
  process_t process;

  fail_unless((process = process_prepare(env, "foobar")) != NULL);
  fail_unless(process_exec(process) == -1);
  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(update_status_null_process) {
  fail_unless(process_update_status(NULL) == -1);
}
END_TEST

START_TEST(is_exited_null_process) {
  fail_unless(process_is_exited(NULL) == -1);
}
END_TEST

START_TEST(get_exit_status_null_process) {
  fail_unless(process_get_exit_status(NULL) == -1);
}
END_TEST

TCase* process_tcase() {
  TCase* tc = tcase_create("process");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, env_register_command_null_env);
  tcase_add_test(tc, env_register_command_null_command);
  tcase_add_test(tc, env_register_command_null_hook);
  tcase_add_test(tc, env_get_command_null_env);
  tcase_add_test(tc, env_get_command_null_command);
  tcase_add_test(tc, env_get_command_found);
  tcase_add_test(tc, env_get_command_not_found);
  tcase_add_test(tc, destroy_null_env);
  tcase_add_test(tc, prepare_null_env);
  tcase_add_test(tc, prepare_null_command);
  tcase_add_test(tc, prepare_no_args);
  tcase_add_test(tc, prepare_one_arg);
  tcase_add_test(tc, prepare_two_args);
  tcase_add_test(tc, process_destroy_null_process);
  tcase_add_test(tc, get_env_success);
  tcase_add_test(tc, get_env_null_process);
  tcase_add_test(tc, get_command_null_process);
  tcase_add_test(tc, get_args_null_process);
  tcase_add_test(tc, get_fd_in_null_process);
  tcase_add_test(tc, get_fd_in);
  tcase_add_test(tc, get_fd_out_null_process);
  tcase_add_test(tc, get_fd_out);
  tcase_add_test(tc, fork_null_process);
  tcase_add_test(tc, fork_absolute_path);
  tcase_add_test(tc, fork_relative_path);
  tcase_add_test(tc, fork_no_such_file);
  tcase_add_test(tc, exec_test_command);
  tcase_add_test(tc, exec_null_process);
  tcase_add_test(tc, exec_absolute_path);
  tcase_add_test(tc, exec_relative_path);
  tcase_add_test(tc, exec_no_hook);
  tcase_add_test(tc, update_status_null_process);
  tcase_add_test(tc, is_exited_null_process);
  tcase_add_test(tc, get_exit_status_null_process);

  return tc;
}

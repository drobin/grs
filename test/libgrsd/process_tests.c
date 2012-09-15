#include <check.h>
#include <stdio.h>

#include "../../src/libgrsd/process.h"
#include "../../src/libgrsd/_session.h"
#include "../libssh_proxy.h"

static session_t session;
static process_env_t env;

static void setup() {
  struct list_head type_list;
  struct list_entry type_entry;

  fail_unless(libssh_proxy_init() == 0);
  fail_unless((session = session_create()) != NULL);

  session_set_state(session, CHANNEL_OPEN);

  type_entry.v.int_val = SSH_REQUEST_CHANNEL_OPEN;
  libssh_proxy_make_list(&type_list, &type_entry, 1);
  libssh_proxy_set_option_list("ssh_message_type", "results", &type_list);

  fail_unless(session_handle(session) == 0);
  fail_unless((env = process_env_create()) != NULL);
}

static void teardown() {
  fail_unless(process_env_destroy(env) == 0);
  fail_unless(session_destroy(session) == 0);
  fail_unless(libssh_proxy_destroy() == 0);
  session = NULL;
}

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

START_TEST(exec_test_command) {
  process_t process;
  int exit_status;
  char buf[64];

  fail_unless((process = process_prepare(env, "test")) != NULL);
  fail_unless(process_exec(process, session) == 0);
  fail_unless(read(process_get_fd_out(process), buf, sizeof(buf)) == 12);
  fail_unless(strncmp(buf, "Hello world!", 12) == 0);
  fail_unless(read(process_get_fd_out(process), buf, sizeof(buf)) == 0);
  fail_unless(process_get_status(process, &exit_status) == 0);
  fail_unless(exit_status == 0);
  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(exec_null_process) {
  fail_unless(process_exec(NULL, session) == -1);
}
END_TEST

START_TEST(exec_null_session) {
  process_t process;

  fail_unless((process = process_prepare(env, "foobar")) != NULL);
  fail_unless(process_exec(process, NULL) == -1);
  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(exec_absolute_path) {
  process_t process;
  int status, exit_status;
  char buf[64];
  size_t nread;

  fail_unless((process = process_prepare(env, "/bin/ls -1")) != NULL);
  fail_unless(process_exec(process, session) == 0);

  while ((status = process_get_status(process, &exit_status)) != 0) {
    fail_unless(status > 0);
  }

  while ((nread = read(process_get_fd_out(process), buf, sizeof(buf))) != 0) {
    fail_unless(nread > 0);
  }

  fail_unless(exit_status == 0);
  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(exec_relative_path) {
  process_t process;
  int status, exit_status;
  char buf[64];
  size_t nread;

  fail_unless((process = process_prepare(env, "ls -1")) != NULL);
  fail_unless(process_exec(process, session) == 0);

  while ((status = process_get_status(process, &exit_status)) != 0) {
    fail_unless(status > 0);
  }

  while ((nread = read(process_get_fd_out(process), buf, sizeof(buf))) != 0) {
    fail_unless(nread > 0);
  }

  fail_unless(exit_status == 0);
  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(exec_no_such_file) {
  process_t process;
  int status, exit_status;
  char buf[64];
  size_t nread;

  fail_unless((process = process_prepare(env, "foobar")) != NULL);
  fail_unless(process_exec(process, session) == 0);

  while ((status = process_get_status(process, &exit_status)) != 0) {
    fail_unless(status > 0);
  }

  while ((nread = read(process_get_fd_out(process), buf, sizeof(buf))) != 0) {
    fail_unless(nread > 0);
  }

  fail_unless(exit_status == 127);
  fail_unless(process_destroy(process) == 0);
}
END_TEST

START_TEST(get_status_null_process) {
  fail_unless(process_get_status(NULL, NULL) == -1);
}
END_TEST

TCase* process_tcase() {
  TCase* tc = tcase_create("process");
  tcase_add_checked_fixture(tc, setup, teardown);

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
  tcase_add_test(tc, exec_test_command);
  tcase_add_test(tc, exec_null_process);
  tcase_add_test(tc, exec_null_session);
  tcase_add_test(tc, exec_absolute_path);
  tcase_add_test(tc, exec_relative_path);
  tcase_add_test(tc, exec_no_such_file);
  tcase_add_test(tc, get_status_null_process);

  return tc;
}

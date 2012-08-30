#include <check.h>
#include <stdio.h>

#include "../../src/libgrsd/process.h"
#include "../../src/libgrsd/_session.h"
#include "../libssh_proxy.h"

static session_t session;

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
}

static void teardown() {
  fail_unless(session_destroy(session) == 0);
  fail_unless(libssh_proxy_destroy() == 0);
  session = NULL;
}

START_TEST(init_null_command) {
  fail_unless(grs_process_init(NULL) == NULL);
}
END_TEST

START_TEST(init_no_args) {
  process_t process;

  fail_unless((process = grs_process_init("foobar")) != NULL);
  fail_unless(strcmp(grs_process_get_command(process), "foobar") == 0);
  fail_unless(grs_process_get_args(process)[0] == NULL);
  fail_unless(grs_process_destroy(process) == 0);
}
END_TEST

START_TEST(init_one_arg) {
  process_t process;

  fail_unless((process = grs_process_init("foobar foo")) != NULL);
  fail_unless(strcmp(grs_process_get_command(process), "foobar") == 0);
  fail_unless(strcmp(grs_process_get_args(process)[0], "foo") == 0);
  fail_unless(grs_process_get_args(process)[1] == NULL);
  fail_unless(grs_process_destroy(process) == 0);
}
END_TEST

START_TEST(init_two_args) {
  process_t process;

  fail_unless((process = grs_process_init("foobar foo bar")) != NULL);
  fail_unless(strcmp(grs_process_get_command(process), "foobar") == 0);
  fail_unless(strcmp(grs_process_get_args(process)[0], "foo") == 0);
  fail_unless(strcmp(grs_process_get_args(process)[1], "bar") == 0);
  fail_unless(grs_process_get_args(process)[2] == NULL);
  fail_unless(grs_process_destroy(process) == 0);
}
END_TEST

START_TEST(destroy_null_process) {
  fail_unless(grs_process_destroy(NULL) == -1);
}
END_TEST

START_TEST(get_command_null_process) {
  fail_unless(grs_process_get_command(NULL) == NULL);
}
END_TEST

START_TEST(get_args_null_process) {
  fail_unless(grs_process_get_args(NULL) == NULL);
}
END_TEST

START_TEST(exec_null_process) {
  fail_unless(grs_process_exec(NULL, session) == -1);
}
END_TEST

START_TEST(exec_null_session) {
  process_t process;

  fail_unless((process = grs_process_init("foobar")) != NULL);
  fail_unless(grs_process_exec(process, NULL) == -1);
  fail_unless(grs_process_destroy(process) == 0);
}
END_TEST

START_TEST(exec_success) {
  process_t process;

  libssh_proxy_set_option_int("ssh_select", "readfds", 1);
  fail_unless((process = grs_process_init("/bin/ls -1")) != NULL);
  fail_unless(grs_process_exec(process, session) == 0);
  fail_unless(libssh_proxy_channel_get_size(session->channel) > 0);
  fail_unless(grs_process_destroy(process) == 0);
}
END_TEST

TCase* process_tcase() {
  TCase* tc = tcase_create("process");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, init_null_command);
  tcase_add_test(tc, init_no_args);
  tcase_add_test(tc, init_one_arg);
  tcase_add_test(tc, init_two_args);
  tcase_add_test(tc, get_command_null_process);
  tcase_add_test(tc, get_args_null_process);
  tcase_add_test(tc, exec_null_process);
  tcase_add_test(tc, exec_null_session);
  tcase_add_test(tc, exec_success);
  tcase_add_test(tc, destroy_null_process);

  return tc;
}

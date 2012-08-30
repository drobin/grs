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

START_TEST(prepare_null_process) {
  fail_unless(grs_process_prepare(NULL, "foo") == -1);
}
END_TEST

START_TEST(prepare_null_command) {
  struct grs_process process;
  fail_unless(grs_process_prepare(&process, NULL) == -1);
}
END_TEST

START_TEST(prepare_success) {
  struct grs_process process;

  fail_unless(grs_process_prepare(&process, "foobar") == 0);
  fail_unless(strcmp(process.command, "foobar") == 0);
}
END_TEST

START_TEST(exec_null_process) {
  fail_unless(grs_process_exec(NULL, session) == -1);
}
END_TEST

START_TEST(exec_null_session) {
  struct grs_process process;
  fail_unless(grs_process_exec(&process, NULL) == -1);
}
END_TEST

START_TEST(exec_success) {
  struct grs_process process;

  libssh_proxy_set_option_int("ssh_select", "readfds", 1);
  fail_unless(grs_process_prepare(&process, "foobar") == 0);
  fail_unless(grs_process_exec(&process, session) == 0);
  fail_unless(libssh_proxy_channel_get_size(session->channel) > 0);
}
END_TEST

TCase* process_tcase() {
  TCase* tc = tcase_create("process");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, prepare_null_process);
  tcase_add_test(tc, prepare_null_command);
  tcase_add_test(tc, prepare_success);
  tcase_add_test(tc, exec_null_process);
  tcase_add_test(tc, exec_null_session);
  tcase_add_test(tc, exec_success);

  return tc;
}

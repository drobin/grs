#include <check.h>

#include "../../src/libgrsd/process.h"

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
  ssh_channel channel;
  fail_unless(grs_process_exec(NULL, channel) == -1);
}
END_TEST

START_TEST(exec_null_channel) {
  struct grs_process process;
  fail_unless(grs_process_exec(&process, NULL) == -1);
}
END_TEST

TCase* process_tcase() {
  TCase* tc = tcase_create("process");

  tcase_add_test(tc, prepare_null_process);
  tcase_add_test(tc, prepare_null_command);
  tcase_add_test(tc, prepare_success);
  tcase_add_test(tc, exec_null_process);
  tcase_add_test(tc, exec_null_channel);

  return tc;
}

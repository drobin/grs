#include <sys/errno.h>
#include <check.h>

#include <grsd.h>

#include "libssh_proxy.h"

static void setup() {
  fail_unless(libssh_proxy_init() == 0);
}

static void teardown() {
  libssh_proxy_destroy();
}

START_TEST(init_destroy) {
  grsd_t handle;
  int result;

  handle = grsd_init();
  fail_unless(handle != NULL);

  result = grsd_destroy(handle);
  fail_unless(result == 0);
}
END_TEST

START_TEST(init_failed) {
  grsd_t handle;

  ssh_proxy_env->ssh_bind_new_should_fail = 1;

  handle = grsd_init();
  fail_unless(handle == NULL);
}
END_TEST

START_TEST(listen_null_handle) {
  int result;

  result = grsd_listen(NULL);
  fail_unless(result == -1);
}
END_TEST

START_TEST(listen_exit_null_handle) {
  int result;

  result = grsd_listen_exit(NULL);
  fail_unless(result == -1);
}
END_TEST

START_TEST(listen_exit) {
  grsd_t handle;
  int result;
  pid_t pid;

  handle = grsd_init();
  fail_unless(handle != NULL);

  pid = check_fork();
  if (pid == 0) {
    fail_unless(grsd_listen_exit(handle) == 0);
  } else if (pid > 0) {
    fail_unless(grsd_listen(handle) == 0);
    check_waitpid_and_exit(pid);
  } else {
    fail(strerror(errno));
  }

  result = grsd_destroy(handle);
  fail_unless(result == 0);
}
END_TEST

START_TEST(get_listen_port_default) {
  grsd_t handle;
  int result;

  handle = grsd_init();
  fail_unless(handle != NULL);

  fail_unless(grsd_get_listen_port(handle) == 22);

  result = grsd_destroy(handle);
  fail_unless(result == 0);
}
END_TEST

START_TEST(get_listen_port_null_handle) {
  fail_unless(grsd_get_listen_port(NULL) == -1);
}
END_TEST

START_TEST(set_listen_port_null_handle) {
  fail_unless(grsd_set_listen_port(NULL, 22) == -1);
}
END_TEST

START_TEST(set_listen_port_out_of_range) {
  fail_unless(grsd_set_listen_port(NULL, 0) == -1);
  fail_unless(grsd_set_listen_port(NULL, 65537) == -1);
}
END_TEST

START_TEST(set_listen_port_failed) {
  grsd_t handle;
  int result;

  handle = grsd_init();
  fail_unless(handle != NULL);

  ssh_proxy_env->ssh_bind_options_set_should_fail = 1;
  fail_unless(grsd_set_listen_port(handle, 22) == -1);

  result = grsd_destroy(handle);
  fail_unless(result == 0);
}
END_TEST

START_TEST(get_set_listen_port) {
  grsd_t handle;
  int result;

  handle = grsd_init();
  fail_unless(handle != NULL);

  fail_unless(grsd_set_listen_port(handle, 4711) == 0);
  fail_unless(grsd_get_listen_port(handle) == 4711);

  result = grsd_destroy(handle);
  fail_unless(result == 0);
}
END_TEST

TCase* libgrsd_tcase() {
  TCase* tc = tcase_create("libgrsd");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, init_destroy);
  tcase_add_test(tc, init_failed);
  tcase_add_test(tc, listen_null_handle);
  tcase_add_test(tc, listen_exit_null_handle);
  tcase_add_test(tc, listen_exit);
  tcase_add_test(tc, get_listen_port_null_handle);
  tcase_add_test(tc, get_listen_port_default);
  tcase_add_test(tc, set_listen_port_null_handle);
  tcase_add_test(tc, set_listen_port_out_of_range);
  tcase_add_test(tc, set_listen_port_failed);
  tcase_add_test(tc, get_set_listen_port);

  return tc;
}

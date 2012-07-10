#include <sys/errno.h>
#include <check.h>
#include <unistd.h>

#include <grsd.h>

#include "libssh_proxy.h"

static grsd_t handle;

static void setup() {
  fail_unless(libssh_proxy_init() == 0);
  fail_unless((handle = grsd_init()) != NULL);
}

static void teardown() {
  fail_unless(grsd_destroy(handle) == 0);
  handle = NULL;
  libssh_proxy_destroy();
}

START_TEST(init_failed) {
  grsd_t handle;

  ssh_proxy_env->ssh_bind_new_should_fail = 1;
  fail_unless((handle = grsd_init()) == NULL);
}
END_TEST

START_TEST(listen_null_handle) {
  int result;

  fail_unless((result = grsd_listen(NULL)) == -1);
}
END_TEST

START_TEST(listen_exit_null_handle) {
  int result;

  fail_unless((result = grsd_listen_exit(NULL)) == -1);
}
END_TEST

START_TEST(listen_exit) {
  pid_t pid = check_fork();

  if (pid == 0) {
    fail_unless(grsd_listen_exit(handle) == 0);
  } else if (pid > 0) {
    fail_unless(grsd_listen(handle) == 0);
    check_waitpid_and_exit(pid);
  } else {
    fail(strerror(errno));
  }
}
END_TEST

START_TEST(listen_foo) {
  pid_t pid = check_fork();
  
  if (pid == 0) {
    int fd = libssh_proxy_connect();
    fail_unless(grsd_listen_exit(handle) == 0);
    close(fd);
  } else if (pid > 0) {
    fail_unless(grsd_listen(handle) == 0);
    check_waitpid_and_exit(pid);
  } else {
    fail(strerror(errno));
  }
}
END_TEST

START_TEST(listen_ssh_new_failed) {
  pid_t pid;
  
  ssh_proxy_env->ssh_new_should_fail = 1;
  
  pid = check_fork();
  
  if (pid == 0) {
    int fd = libssh_proxy_connect();
    fail_unless(grsd_listen_exit(handle) == 0);
    close(fd);
  } else if (pid > 0) {
    fail_unless(grsd_listen(handle) == -1);
    check_waitpid_and_exit(pid);
  } else {
    fail(strerror(errno));
  }
}
END_TEST

START_TEST(listen_bind_failed) {
  ssh_proxy_env->ssh_bind_listen_should_fail = 1;
  fail_unless(grsd_listen(handle) == -1);
}
END_TEST

START_TEST(get_listen_port_default) {
  fail_unless(grsd_get_listen_port(handle) == 22);
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
  ssh_proxy_env->ssh_bind_options_set_should_fail = 1;
  fail_unless(grsd_set_listen_port(handle, 22) == -1);
}
END_TEST

START_TEST(get_set_listen_port) {
  fail_unless(grsd_set_listen_port(handle, 4711) == 0);
  fail_unless(grsd_get_listen_port(handle) == 4711);
}
END_TEST

START_TEST(get_hostkey_null_handler) {
  fail_unless(grsd_get_hostkey(NULL) == NULL);
}
END_TEST

START_TEST(get_hostkey_default) {
  fail_unless(grsd_get_hostkey(handle) == NULL);
}
END_TEST

START_TEST(set_hostkey_null_handler) {
  fail_unless(grsd_set_hostkey(NULL, "/path/to/key") == -1);
}
END_TEST

START_TEST(set_hostkey_null_path) {
  fail_unless(grsd_set_hostkey(handle, NULL) == -1);
}
END_TEST

START_TEST(set_hostkey_failed) {
  ssh_proxy_env->ssh_bind_options_set_should_fail = 1;
  fail_unless(grsd_set_hostkey(handle, "/path/to/key") == -1);
}
END_TEST

START_TEST(get_set_hostkey) {
  fail_unless(grsd_set_hostkey(handle, "/path/to/key") == 0);
  fail_unless(strcmp(grsd_get_hostkey(handle), "/path/to/key") == 0);
}
END_TEST

TCase* libgrsd_tcase() {
  TCase* tc = tcase_create("libgrsd");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, init_failed);
  tcase_add_test(tc, listen_null_handle);
  tcase_add_test(tc, listen_exit_null_handle);
  tcase_add_test(tc, listen_exit);
  tcase_add_test(tc, listen_foo);
  tcase_add_test(tc, listen_ssh_new_failed);
  tcase_add_test(tc, listen_bind_failed);
  tcase_add_test(tc, get_listen_port_null_handle);
  tcase_add_test(tc, get_listen_port_default);
  tcase_add_test(tc, set_listen_port_null_handle);
  tcase_add_test(tc, set_listen_port_out_of_range);
  tcase_add_test(tc, set_listen_port_failed);
  tcase_add_test(tc, get_set_listen_port);
  tcase_add_test(tc, get_hostkey_null_handler);
  tcase_add_test(tc, get_hostkey_default);
  tcase_add_test(tc, set_hostkey_null_handler);
  tcase_add_test(tc, set_hostkey_null_path);
  tcase_add_test(tc, set_hostkey_failed);
  tcase_add_test(tc, get_set_hostkey);

  return tc;
}

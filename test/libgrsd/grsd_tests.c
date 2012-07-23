#include <sys/errno.h>
#include <check.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <libssh/libssh.h>
#include <grsd.h>

#include "../hostkey.h"

static grsd_t handle;

static void setup() {
  fail_unless((handle = grsd_init()) != NULL);
}

static void teardown() {
  fail_unless(grsd_destroy(handle) == 0);
  handle = NULL;
}

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
    grsd_set_listen_port(handle, 4711);
    fail_unless(grsd_listen(handle) == 0);
    check_waitpid_and_exit(pid);
  } else {
    fail(strerror(errno));
  }
}
END_TEST

START_TEST(listen_connect) {
  pid_t pid = check_fork();
  
  if (pid == 0) {
    ssh_session session = ssh_new();
    ssh_channel channel;
    char buf[512];
    size_t nread;
    
    ssh_options_set(session, SSH_OPTIONS_HOST, "localhost");
    ssh_options_set(session, SSH_OPTIONS_PORT_STR, "4711");;

    ssh_connect(session);
    
    ssh_userauth_password(session, "foo", "foo");
    channel = ssh_channel_new(session);
    ssh_channel_open_session(channel);
    ssh_channel_request_exec(channel, "ls");
    while ((nread = ssh_channel_read(channel, buf, sizeof(buf), 0)) != -1);
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    ssh_disconnect(session);
    ssh_free(session);
    grsd_listen_exit(handle);
  } else if (pid > 0) {
    grsd_set_listen_port(handle, 4711);
    grsd_set_hostkey(handle, hostkey_get_path());
    fail_unless(grsd_listen(handle) == 0);
    check_waitpid_and_exit(pid);
  } else {
    fail(strerror(errno));
  }
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

START_TEST(get_set_hostkey) {
  fail_unless(grsd_set_hostkey(handle, "/path/to/key") == 0);
  fail_unless(strcmp(grsd_get_hostkey(handle), "/path/to/key") == 0);
}
END_TEST

TCase* grsd_tcase() {
  TCase* tc = tcase_create("grsd");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, listen_null_handle);
  tcase_add_test(tc, listen_exit_null_handle);
  tcase_add_test(tc, listen_exit);
  tcase_add_test(tc, listen_connect);
  tcase_add_test(tc, get_listen_port_null_handle);
  tcase_add_test(tc, get_listen_port_default);
  tcase_add_test(tc, set_listen_port_null_handle);
  tcase_add_test(tc, set_listen_port_out_of_range);
  tcase_add_test(tc, get_set_listen_port);
  tcase_add_test(tc, get_hostkey_null_handler);
  tcase_add_test(tc, get_hostkey_default);
  tcase_add_test(tc, set_hostkey_null_handler);
  tcase_add_test(tc, set_hostkey_null_path);
  tcase_add_test(tc, get_set_hostkey);

  return tc;
}

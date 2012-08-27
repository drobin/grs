#include <stdlib.h>

#include <check.h>

#include <libssh/libssh.h>

#include "../../src/libgrsd/session.h"
#include "../libssh_proxy.h"

static struct _grsd* handle;
static session_t session;

static void setup() {
  fail_unless(libssh_proxy_init() == 0);
  fail_unless((handle = grsd_init()) != NULL);
  fail_unless((session = session_create(handle)) != NULL);
}

static void teardown() {
  fail_unless(session_destroy(session) == 0);
  fail_unless(grsd_destroy(handle) == 0);
  session = NULL;
  handle = NULL;
  fail_unless(libssh_proxy_destroy() == 0);
}

START_TEST(create_ssh_new_failure) {
  libssh_proxy_set_option_int("ssh_new", "fail", 1);
  fail_unless(session_create() == NULL);
}
END_TEST

START_TEST(destroy_null_handle) {
  fail_unless(session_destroy(NULL) == -1);
}
END_TEST

START_TEST(get_state_null_handle) {
  fail_unless(session_get_state(NULL) == -1);
}
END_TEST

START_TEST(get_state_initial) {
  fail_unless(session_get_state(session) == AUTH);
}
END_TEST

START_TEST(set_state_null_handle) {
  fail_unless(session_set_state(NULL, NOP) == -1);
}
END_TEST

START_TEST(get_set_state) {
  fail_unless(session_get_state(session) == AUTH);
  fail_unless(session_set_state(session, CHANNEL_OPEN) == 0);
  fail_unless(session_get_state(session) == CHANNEL_OPEN);
}
END_TEST

START_TEST(accept_null_session) {
  fail_unless(session_accept(NULL, handle) == -1);
}
END_TEST

START_TEST(accept_null_handle) {
  fail_unless(session_accept(session, NULL) == -1);
}
END_TEST

START_TEST(accept_ssh_bind_accept_failure) {
  libssh_proxy_set_option_int("ssh_bind_accept", "fail", 1);
  fail_unless(session_accept(session, handle) == -1);
}
END_TEST

START_TEST(accept_ssh_handle_key_exchange_failure) {
  libssh_proxy_set_option_int("ssh_handle_key_exchange", "fail", 1);
  fail_unless(session_accept(session, handle) == -1);
}
END_TEST

START_TEST(accept_established) {
  fail_unless(session_accept(session, handle) == 0);
}
END_TEST

START_TEST(get_fd_null_session) {
  fail_unless(session_get_fd(NULL) == -1);
}
END_TEST

START_TEST(get_fd_not_connected) {
  libssh_proxy_set_option_int("ssh_is_connected", "connected", 0);
  fail_unless(session_get_fd(session) == -1);
}
END_TEST

START_TEST(get_fd_succes) {
  libssh_proxy_set_option_int("ssh_get_fd", "fd", 4711);
  fail_unless(session_get_fd(session) == 4711);
}
END_TEST

START_TEST(handle_null_session) {
  fail_unless(session_handle(NULL) == -1);
}
END_TEST

START_TEST(handle_invalid_msg) {
  libssh_proxy_set_option_int("ssh_message_get", "fail", 1);
  fail_unless(session_handle(session) == -1);
}
END_TEST

START_TEST(handle_invalid_msg_type) {
  struct list_head list;
  struct list_entry entry;

  entry.v.int_val = -1;
  libssh_proxy_make_list(&list, &entry, 1);
  libssh_proxy_set_option_list("ssh_message_type", "results", &list);

  fail_unless(session_handle(session) == -1);
}
END_TEST

START_TEST(handle_auth_invalid_msg_type) {
  struct list_head list;
  struct list_entry entry;

  entry.v.int_val = SSH_REQUEST_AUTH + 1;
  libssh_proxy_make_list(&list, &entry, 1);
  libssh_proxy_set_option_list("ssh_message_type", "results", &list);

  fail_unless(session_handle(session) == 0);
  fail_unless(session_get_state(session) == AUTH);
}
END_TEST

START_TEST(handle_auth_invalid_msg_subtype) {
  struct list_head type_list, subtype_list;
  struct list_entry type_entry, subtype_entry;

  type_entry.v.int_val = SSH_REQUEST_AUTH;
  libssh_proxy_make_list(&type_list, &type_entry, 1);
  libssh_proxy_set_option_list("ssh_message_type", "results", &type_list);

  subtype_entry.v.int_val = SSH_AUTH_METHOD_PASSWORD + 1;
  libssh_proxy_make_list(&subtype_list, &subtype_entry, 1);
  libssh_proxy_set_option_list("ssh_message_subtype", "results", &subtype_list);

  fail_unless(session_handle(session) == 0);
  fail_unless(session_get_state(session) == AUTH);
}
END_TEST

START_TEST(handle_auth_wrong_password) {
  struct list_head type_list, subtype_list;
  struct list_entry type_entry, subtype_entry;

  type_entry.v.int_val = SSH_REQUEST_AUTH;
  libssh_proxy_make_list(&type_list, &type_entry, 1);
  libssh_proxy_set_option_list("ssh_message_type", "results", &type_list);

  subtype_entry.v.int_val = SSH_AUTH_METHOD_PASSWORD;
  libssh_proxy_make_list(&subtype_list, &subtype_entry, 1);
  libssh_proxy_set_option_list("ssh_message_subtype", "results", &subtype_list);

  libssh_proxy_set_option_string("ssh_message_auth_user", "result", "foo");
  libssh_proxy_set_option_string("ssh_message_auth_password", "result", "bar");

  fail_unless(session_handle(session) == 0);
  fail_unless(session_get_state(session) == AUTH);
}
END_TEST

START_TEST(handle_auth_success) {
  struct list_head type_list, subtype_list;
  struct list_entry type_entry, subtype_entry;

  type_entry.v.int_val = SSH_REQUEST_AUTH;
  libssh_proxy_make_list(&type_list, &type_entry, 1);
  libssh_proxy_set_option_list("ssh_message_type", "results", &type_list);

  subtype_entry.v.int_val = SSH_AUTH_METHOD_PASSWORD;
  libssh_proxy_make_list(&subtype_list, &subtype_entry, 1);
  libssh_proxy_set_option_list("ssh_message_subtype", "results", &subtype_list);

  libssh_proxy_set_option_string("ssh_message_auth_user", "result", "foo");
  libssh_proxy_set_option_string("ssh_message_auth_password", "result", "foo");

  fail_unless(session_handle(session) == 0);
  fail_unless(session_get_state(session) == CHANNEL_OPEN);
}
END_TEST

START_TEST(handle_channel_open_invalid_msg_type) {
  struct list_head type_list;
  struct list_entry type_entry;

  session_set_state(session, CHANNEL_OPEN);

  type_entry.v.int_val = SSH_REQUEST_CHANNEL_OPEN + 1;
  libssh_proxy_make_list(&type_list, &type_entry, 1);
  libssh_proxy_set_option_list("ssh_message_type", "results", &type_list);

  fail_unless(session_handle(session) == 0);
  fail_unless(session_get_state(session) == CHANNEL_OPEN);
}
END_TEST

START_TEST(handle_channel_open_no_channel) {
  struct list_head type_list;
  struct list_entry type_entry;

  session_set_state(session, CHANNEL_OPEN);

  type_entry.v.int_val = SSH_REQUEST_CHANNEL_OPEN;
  libssh_proxy_make_list(&type_list, &type_entry, 1);
  libssh_proxy_set_option_list("ssh_message_type", "results", &type_list);
  libssh_proxy_set_option_int("ssh_message_channel_request_open_reply_accept",
                              "fail", 1);

  fail_unless(session_handle(session) == -1);
  fail_unless(session_get_state(session) == CHANNEL_OPEN);
}
END_TEST

START_TEST(handle_channel_open_success) {
  struct list_head type_list;
  struct list_entry type_entry;

  session_set_state(session, CHANNEL_OPEN);

  type_entry.v.int_val = SSH_REQUEST_CHANNEL_OPEN;
  libssh_proxy_make_list(&type_list, &type_entry, 1);
  libssh_proxy_set_option_list("ssh_message_type", "results", &type_list);

  fail_unless(session_handle(session) == 0);
  fail_unless(session_get_state(session) == REQUEST_CHANNEL);
}
END_TEST

START_TEST(handle_request_channel_invalid_msg_type) {
  struct list_head type_list;
  struct list_entry type_entry;

  session_set_state(session, REQUEST_CHANNEL);

  type_entry.v.int_val = SSH_REQUEST_CHANNEL + 1;
  libssh_proxy_make_list(&type_list, &type_entry, 1);
  libssh_proxy_set_option_list("ssh_message_type", "results", &type_list);

  fail_unless(session_handle(session) == 0);
  fail_unless(session_get_state(session) == REQUEST_CHANNEL);
}
END_TEST

START_TEST(handle_request_channel_invalid_msg_subtype) {
  struct list_head type_list, subtype_list;
  struct list_entry type_entry, subtype_entry;

  session_set_state(session, REQUEST_CHANNEL);

  type_entry.v.int_val = SSH_REQUEST_CHANNEL;
  libssh_proxy_make_list(&type_list, &type_entry, 1);
  libssh_proxy_set_option_list("ssh_message_type", "results", &type_list);

  subtype_entry.v.int_val = SSH_CHANNEL_REQUEST_EXEC + 1;
  libssh_proxy_make_list(&subtype_list, &subtype_entry, 1);
  libssh_proxy_set_option_list("ssh_message_subtype", "results", &subtype_list);

  fail_unless(session_handle(session) == 1);
  fail_unless(session_get_state(session) == REQUEST_CHANNEL);
}
END_TEST

START_TEST(handle_request_channel_success) {
  struct list_head type_list, subtype_list;
  struct list_entry type_entry, subtype_entry;

  session_set_state(session, REQUEST_CHANNEL);

  type_entry.v.int_val = SSH_REQUEST_CHANNEL;
  libssh_proxy_make_list(&type_list, &type_entry, 1);
  libssh_proxy_set_option_list("ssh_message_type", "results", &type_list);

  subtype_entry.v.int_val = SSH_CHANNEL_REQUEST_EXEC;
  libssh_proxy_make_list(&subtype_list, &subtype_entry, 1);
  libssh_proxy_set_option_list("ssh_message_subtype", "results", &subtype_list);

  fail_unless(session_handle(session) == 1);
  fail_unless(session_get_state(session) == NOP);
}
END_TEST

TCase* session_tcase() {
  TCase* tc = tcase_create("session");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, create_ssh_new_failure);
  tcase_add_test(tc, destroy_null_handle);
  tcase_add_test(tc, get_state_null_handle);
  tcase_add_test(tc, get_state_initial);
  tcase_add_test(tc, set_state_null_handle);
  tcase_add_test(tc, get_set_state);
  tcase_add_test(tc, accept_null_session);
  tcase_add_test(tc, accept_null_handle);
  tcase_add_test(tc, accept_ssh_bind_accept_failure);
  tcase_add_test(tc, accept_ssh_handle_key_exchange_failure);
  tcase_add_test(tc, accept_established);
  tcase_add_test(tc, get_fd_null_session);
  tcase_add_test(tc, get_fd_not_connected);
  tcase_add_test(tc, get_fd_succes);
  tcase_add_test(tc, handle_null_session);
  tcase_add_test(tc, handle_invalid_msg);
  tcase_add_test(tc, handle_invalid_msg_type);
  tcase_add_test(tc, handle_auth_invalid_msg_type);
  tcase_add_test(tc, handle_auth_invalid_msg_subtype);
  tcase_add_test(tc, handle_auth_wrong_password);
  tcase_add_test(tc, handle_auth_success);
  tcase_add_test(tc, handle_channel_open_invalid_msg_type);
  tcase_add_test(tc, handle_channel_open_no_channel);
  tcase_add_test(tc, handle_channel_open_success);
  tcase_add_test(tc, handle_request_channel_invalid_msg_type);
  tcase_add_test(tc, handle_request_channel_invalid_msg_subtype);
  tcase_add_test(tc, handle_request_channel_success);

  return tc;
}

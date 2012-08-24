#include <stdlib.h>

#include <check.h>

#include <grsd.h>
#include <types.h>

#include "../libssh_proxy.h"

static struct _grsd* handle;
static session_t session;

static void setup() {
  fail_unless(libssh_proxy_init() == 0);
  fail_unless((handle = malloc(sizeof(struct _grsd))) != NULL);
  fail_unless((session = session_create(handle)) != NULL);
  fail_unless(session_get_grsd(session) == handle);
}

static void teardown() {
  fail_unless(session_destroy(session) == 0);
  free(handle);
  session = NULL;
  handle = NULL;
  fail_unless(libssh_proxy_destroy() == 0);
}

START_TEST(create_null_handle) {
  fail_unless(session_create(NULL) == NULL);
}
END_TEST

START_TEST(create_ssh_new_failure) {
  libssh_proxy_set_option_int("ssh_new", "fail", 1);
  fail_unless(session_create(handle) == NULL);
}
END_TEST

START_TEST(destroy_null_handle) {
  fail_unless(session_destroy(NULL) == -1);
}
END_TEST

START_TEST(get_grst_null_handle) {
  fail_unless(session_get_grsd(NULL) == NULL);
}
END_TEST

START_TEST(get_grsd) {
  fail_unless(session_get_grsd(session) == handle);
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

START_TEST(accept_null_handle) {
  fail_unless(session_accept(NULL) == -1);
}
END_TEST

START_TEST(accept_ssh_bind_accept_failure) {
  libssh_proxy_set_option_int("ssh_bind_accept", "fail", 1);
  fail_unless(session_accept(session) == -1);
}
END_TEST

START_TEST(accept_ssh_handle_key_exchange_failure) {
  libssh_proxy_set_option_int("ssh_handle_key_exchange", "fail", 1);
  fail_unless(session_accept(session) == -1);
}
END_TEST

START_TEST(accept_established) {
  fail_unless(session_accept(session) == 0);
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

START_TEST(handle_success) {
  struct list_head list;
  struct list_entry entry;

  entry.v.int_val = 1;
  libssh_proxy_make_list(&list, &entry, 1);
  libssh_proxy_set_option_list("ssh_message_type", "results", &list);

  fail_unless(session_handle(session) == 0);
}
END_TEST

TCase* session_tcase() {
  TCase* tc = tcase_create("session");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, create_null_handle);
  tcase_add_test(tc, create_ssh_new_failure);
  tcase_add_test(tc, destroy_null_handle);
  tcase_add_test(tc, get_grst_null_handle);
  tcase_add_test(tc, get_grsd);
  tcase_add_test(tc, get_state_null_handle);
  tcase_add_test(tc, get_state_initial);
  tcase_add_test(tc, set_state_null_handle);
  tcase_add_test(tc, get_set_state);
  tcase_add_test(tc, accept_null_handle);
  tcase_add_test(tc, accept_ssh_bind_accept_failure);
  tcase_add_test(tc, accept_ssh_handle_key_exchange_failure);
  tcase_add_test(tc, accept_established);
  tcase_add_test(tc, handle_null_session);
  tcase_add_test(tc, handle_invalid_msg);
  tcase_add_test(tc, handle_invalid_msg_type);
  tcase_add_test(tc, handle_success);

  return tc;
}

#include <check.h>
#include <stdlib.h>

#include <handler.h>
#include <types.h>

#include "../libssh_proxy.h"

static struct _grsd* handle;
static session_t session;
static int fds[2];
static struct event_base* eb;
static struct event* ev;

static void setup() {
  fail_unless(libssh_proxy_init() == 0);
  fail_unless((handle = malloc(sizeof(struct _grsd))) != NULL);
  fail_unless((session = session_create(handle)) != NULL);
  fail_unless(pipe(fds) == 0);
  fail_unless((eb = event_base_new()) != NULL);
  ev = event_new(eb, fds[0], EV_READ, grsd_handle_session, session);
  fail_unless(ev != NULL);

  event_add(ev, NULL);
  handle->event_base = eb;
}

static void teardown() {
  event_free(ev);
  event_base_free(eb);
  close(fds[0]);
  close(fds[1]);
  free(handle);
  fail_unless(libssh_proxy_destroy() == 0);
}

START_TEST(invalid_message_type) {
  struct list_head list;
  struct list_entry entry;

  entry.v.int_val = -1;
  libssh_proxy_make_list(&list, &entry, 1);
  libssh_proxy_set_option_list("ssh_message_type", "results", &list);

  event_active(ev, EV_READ, 1);
  fail_unless(event_base_loop(eb, EVLOOP_ONCE) == 0);

  // TODO The session is destroyed here, how to check this?
}
END_TEST

START_TEST(handle_auth_invalid_msg_type) {
  struct list_head list;
  struct list_entry entries[2];

  entries[0].v.int_val = 1;
  entries[1].v.int_val = SSH_REQUEST_AUTH + 1;
  libssh_proxy_make_list(&list, entries, 2);
  libssh_proxy_set_option_list("ssh_message_type", "results", &list);

  event_active(ev, EV_READ, 1);
  fail_unless(event_base_loop(eb, EVLOOP_ONCE) == 0);

  fail_unless(session_get_state(session) == AUTH);
  session_destroy(session); // session is not destroyed
}
END_TEST

START_TEST(handle_auth_invalid_msg_subtype) {
  struct list_head type_list, subtype_list;
  struct list_entry type_entries[2], subtype_entry;

  type_entries[0].v.int_val = 1;
  type_entries[1].v.int_val = SSH_REQUEST_AUTH;
  libssh_proxy_make_list(&type_list, type_entries, 2);
  libssh_proxy_set_option_list("ssh_message_type", "results", &type_list);

  subtype_entry.v.int_val = SSH_AUTH_METHOD_PASSWORD + 1;
  libssh_proxy_make_list(&subtype_list, &subtype_entry, 1);
  libssh_proxy_set_option_list("ssh_message_subtype", "results", &subtype_list);

  event_active(ev, EV_READ, 1);
  fail_unless(event_base_loop(eb, EVLOOP_ONCE) == 0);

  fail_unless(session_get_state(session) == AUTH);
  session_destroy(session); // session is not destroyed
}
END_TEST

START_TEST(handle_auth_wrong_password) {
  struct list_head type_list, subtype_list;
  struct list_entry type_entries[2], subtype_entry;

  type_entries[0].v.int_val = 1;
  type_entries[1].v.int_val = SSH_REQUEST_AUTH;
  libssh_proxy_make_list(&type_list, type_entries, 2);
  libssh_proxy_set_option_list("ssh_message_type", "results", &type_list);

  subtype_entry.v.int_val = SSH_AUTH_METHOD_PASSWORD;
  libssh_proxy_make_list(&subtype_list, &subtype_entry, 1);
  libssh_proxy_set_option_list("ssh_message_subtype", "results", &subtype_list);

  libssh_proxy_set_option_string("ssh_message_auth_user", "result", "foo");
  libssh_proxy_set_option_string("ssh_message_auth_password", "result", "bar");

  event_active(ev, EV_READ, 1);
  fail_unless(event_base_loop(eb, EVLOOP_ONCE) == 0);

  fail_unless(session_get_state(session) == AUTH);
  session_destroy(session); // session is not destroyed
}
END_TEST

START_TEST(handle_auth_success) {
  struct list_head type_list, subtype_list;
  struct list_entry type_entries[2], subtype_entry;

  type_entries[0].v.int_val = 1;
  type_entries[1].v.int_val = SSH_REQUEST_AUTH;
  libssh_proxy_make_list(&type_list, type_entries, 2);
  libssh_proxy_set_option_list("ssh_message_type", "results", &type_list);

  subtype_entry.v.int_val = SSH_AUTH_METHOD_PASSWORD;
  libssh_proxy_make_list(&subtype_list, &subtype_entry, 1);
  libssh_proxy_set_option_list("ssh_message_subtype", "results", &subtype_list);

  libssh_proxy_set_option_string("ssh_message_auth_user", "result", "foo");
  libssh_proxy_set_option_string("ssh_message_auth_password", "result", "foo");

  event_active(ev, EV_READ, 1);
  fail_unless(event_base_loop(eb, EVLOOP_ONCE) == 0);

  fail_unless(session_get_state(session) == CHANNEL_OPEN);
  session_destroy(session); // session is not destroyed
}
END_TEST

TCase* handle_session_tcase() {
  TCase* tc = tcase_create("handle_session");

  tcase_add_checked_fixture(tc, setup, teardown);
  tcase_add_test(tc, invalid_message_type);
  tcase_add_test(tc, handle_auth_invalid_msg_type);
  tcase_add_test(tc, handle_auth_invalid_msg_subtype);
  tcase_add_test(tc, handle_auth_wrong_password);
  tcase_add_test(tc, handle_auth_success);

  return tc;
}

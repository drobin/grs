#include <check.h>

#include "../../src/libgrsd/session2.h"

static session2_t session;

static void setup() {
  fail_unless((session = session2_create()) != NULL);
}

static void teardown() {
  fail_unless(session2_destroy(session) == 0);
  session = NULL;
}

START_TEST(destroy_null_session) {
  fail_unless(session2_destroy(NULL) == -1);
}
END_TEST

START_TEST(get_state_null_session) {
  fail_unless(session2_get_state(NULL) == -1);
}
END_TEST

START_TEST(get_state_initial) {
  fail_unless(session2_get_state(session) == NEED_AUTHENTICATION);
}
END_TEST

START_TEST(set_state_null_session) {
  fail_unless(session2_set_state(NULL, NOOP) == -1);
}
END_TEST

START_TEST(get_set_state) {
  fail_unless(session2_get_state(session) == NEED_AUTHENTICATION);
  fail_unless(session2_set_state(session, NOOP) == 0);
  fail_unless(session2_get_state(session) == NOOP);
}
END_TEST

START_TEST(authenticate_null_session) {
  fail_unless(session2_authenticate(NULL, "foo", "foo") == -1);
}
END_TEST

START_TEST(authenticate_null_username) {
  fail_unless(session2_authenticate(session, NULL, "foo") == -1);
}
END_TEST

START_TEST(authenticate_null_password) {
  fail_unless(session2_authenticate(session, "foo", NULL) == -1);
}
END_TEST

START_TEST(authenticate_wrong_state) {
  fail_unless(session2_set_state(session, NOOP) == 0);
  fail_unless(session2_authenticate(session, "foo", "foo") == -1);
}
END_TEST

START_TEST(authenticate_wrong_password) {
  fail_unless(session2_authenticate(session, "foo", "bar") == -1);
  fail_unless(session2_get_state(session) == NEED_AUTHENTICATION);
}
END_TEST

START_TEST(authenticate_success) {
  fail_unless(session2_authenticate(session, "foo", "foo") == 0);
  fail_unless(session2_get_state(session) == NOOP);
}
END_TEST

TCase* session2_tcase() {
  TCase* tc = tcase_create("session");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, destroy_null_session);
  tcase_add_test(tc, get_state_null_session);
  tcase_add_test(tc, get_state_initial);
  tcase_add_test(tc, set_state_null_session);
  tcase_add_test(tc, get_set_state);
  tcase_add_test(tc, authenticate_null_session);
  tcase_add_test(tc, authenticate_null_username);
  tcase_add_test(tc, authenticate_null_password);
  tcase_add_test(tc, authenticate_wrong_state);
  tcase_add_test(tc, authenticate_wrong_password);
  tcase_add_test(tc, authenticate_success);

  return tc;
}

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
  fail_unless(session2_get_state(session) == NOOP);
}
END_TEST

TCase* session2_tcase() {
  TCase* tc = tcase_create("session");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, destroy_null_session);
  tcase_add_test(tc, get_state_null_session);
  tcase_add_test(tc, get_state_initial);

  return tc;
}

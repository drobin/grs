#include <check.h>

#include <session.h>

static session_t session;

static void setup() {
  fail_unless((session = session_create()) != NULL);
}

static void teardown() {
  fail_unless(session_destroy(session) == 0);
  session = NULL;
}

START_TEST(destroy_null_handle) {
  fail_unless(session_destroy(NULL) == -1);
}
END_TEST

TCase* session_tcase() {
  TCase* tc = tcase_create("session");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, destroy_null_handle);
  
  return tc;
}

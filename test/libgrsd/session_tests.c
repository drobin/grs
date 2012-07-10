#include <check.h>

#include <grsd.h>
#include <session.h>

static grsd_t handle;
static session_t session;

static void setup() {
  fail_unless((handle = grsd_init()) != NULL);
  fail_unless((session = session_create(handle)) != NULL);
}

static void teardown() {
  fail_unless(session_destroy(session) == 0);
  fail_unless(grsd_destroy(handle) == 0);
  session = NULL;
  handle = NULL;
}

START_TEST(create_null_handle) {
  fail_unless(session_create(NULL) == NULL);
}
END_TEST

START_TEST(destroy_null_handle) {
  fail_unless(session_destroy(NULL) == -1);
}
END_TEST

TCase* session_tcase() {
  TCase* tc = tcase_create("session");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, create_null_handle);
  tcase_add_test(tc, destroy_null_handle);
  
  return tc;
}

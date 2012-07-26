#include <check.h>

#include <grsd.h>

//static grsd_t handle;
//static session_t session;

#if 0
static void setup() {
  fail_unless((handle = grsd_init()) != NULL);
  fail_unless((session = session_create(handle)) != NULL);
  fail_unless(session_get_grsd(session) == handle);
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

START_TEST(get_grst_null_handle) {
  fail_unless(session_get_grsd(NULL) == NULL);
}
END_TEST

START_TEST(accept_null_handle) {
  fail_unless(session_accept(NULL) == -1);
}
END_TEST
#endif

TCase* session_tcase() {
  TCase* tc = tcase_create("session");
  //tcase_add_checked_fixture(tc, setup, teardown);

#if 0
  tcase_add_test(tc, create_null_handle);
  tcase_add_test(tc, destroy_null_handle);
  tcase_add_test(tc, get_grst_null_handle);
  tcase_add_test(tc, accept_null_handle);
#endif

  return tc;
}

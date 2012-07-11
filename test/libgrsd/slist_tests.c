#include <check.h>

#include <grsd.h>
#include <session.h>
#include <slist.h>

static slist_t slist;

static void setup() {
  fail_unless((slist = slist_init()) != NULL);
  fail_unless(slist_get_size(slist) == 0);
}

static void teardown() {
  fail_unless(slist_destroy(slist) == 0);
  slist = NULL;
}

START_TEST(destroy_null_handle) {
  fail_unless(slist_destroy(NULL) == -1);
}
END_TEST

START_TEST(get_size_null_slist) {
  fail_unless(slist_get_size(NULL) == -1);
}
END_TEST

START_TEST(prepend_null_slist) {
  grsd_t handle = grsd_init();
  session_t session = session_create(handle);
  
  fail_unless(slist_prepend(NULL, session));
  
  session_destroy(session);
  grsd_destroy(handle);
}
END_TEST

START_TEST(prepend_null_session) {
  fail_unless(slist_prepend(slist, NULL));
}
END_TEST


TCase* slist_tcase() {
  TCase* tc = tcase_create("slist");
  tcase_add_checked_fixture(tc, setup, teardown);
  
  tcase_add_test(tc, destroy_null_handle);
  tcase_add_test(tc, get_size_null_slist);
  tcase_add_test(tc, prepend_null_slist);
  tcase_add_test(tc, prepend_null_session);
  
  return tc;
}
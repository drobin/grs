#include <check.h>

#include <slist.h>

static slist_t slist;

static void setup() {
  fail_unless((slist = slist_init()) != NULL);
}

static void teardown() {
  fail_unless(slist_destroy(slist) == 0);
  slist = NULL;
}

START_TEST(destroy_null_handle) {
  fail_unless(slist_destroy(NULL) == -1);
}
END_TEST

TCase* slist_tcase() {
  TCase* tc = tcase_create("slist");
  tcase_add_checked_fixture(tc, setup, teardown);
  
  tcase_add_test(tc, destroy_null_handle);
  
  return tc;
}
#include <check.h>

#include <grsd.h>

START_TEST(init_destroy) {
  grsd_t handle;
  int result;

  handle = grsd_init();
  fail_unless(handle != NULL);

  result = grsd_destroy(handle);
  fail_unless(result == 0);
}
END_TEST

Suite* libgrsd_suite() {
  Suite* s = suite_create("libgrsd_test");

  TCase* tc = tcase_create("libgrsd");
  suite_add_tcase(s, tc);

  tcase_add_test(tc, init_destroy);

  return s;
}

int main(int argc, char** argv) {
  int nfailed;

  Suite* s = libgrsd_suite();
  SRunner* sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);

  nfailed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (nfailed == 0) ? 0 : 1;
}

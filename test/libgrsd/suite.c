#include <check.h>

Suite* libgrsd_suite() {
  Suite* s = suite_create("libgrsd_test");
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

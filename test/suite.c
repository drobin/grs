#include <check.h>

extern TCase* libgrsd_tcase();

static Suite* grs_suite() {
  Suite* s = suite_create("grs_test");

  suite_add_tcase(s, libgrsd_tcase());

  return s;
}

int main(int argc, char** argv) {
  int nfailed;

  Suite* s = grs_suite();
  SRunner* sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);

  nfailed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (nfailed == 0) ? 0 : 1;
}

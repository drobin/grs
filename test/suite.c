#include <check.h>

#include "hostkey.h"

extern TCase* grsd_tcase();
extern TCase* session_tcase();

static Suite* grs_suite() {
  Suite* s = suite_create("grs_test");

  suite_add_tcase(s, grsd_tcase());
  suite_add_tcase(s, session_tcase());

  return s;
}

int main(int argc, char** argv) {
  int nfailed;

  hostkey_generate();
  
  Suite* s = grs_suite();
  SRunner* sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);

  nfailed = srunner_ntests_failed(sr);
  srunner_free(sr);

  hostkey_remove();
  
  return (nfailed == 0) ? 0 : 1;
}

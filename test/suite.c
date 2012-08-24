#include <check.h>

#include "hostkey.h"

extern TCase* grsd_tcase();
extern TCase* process_tcase();
extern TCase* session_tcase();
extern TCase* slist_tcase();

static Suite* grs_suite() {
  Suite* s = suite_create("grs_test");

  suite_add_tcase(s, grsd_tcase());
  suite_add_tcase(s, process_tcase());
  suite_add_tcase(s, session_tcase());
  suite_add_tcase(s, slist_tcase());

  return s;
}

int main(int argc, char** argv) {
  int nfailed;

  Suite* s = grs_suite();
  SRunner* sr = srunner_create(s);

  #ifdef ENABLE_DEBUG
  srunner_set_fork_status(sr, CK_NOFORK);
  #endif

  srunner_run_all(sr, CK_NORMAL);

  nfailed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (nfailed == 0) ? 0 : 1;
}

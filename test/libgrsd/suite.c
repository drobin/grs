#include <stdlib.h>
#include <check.h>

#include <grsd.h>

#include "libssh_proxy.h"

static void setup() {
  fail_unless(libssh_proxy_init() == 0);
}

static void teardown() {
  libssh_proxy_destroy();
}

START_TEST(init_destroy) {
  grsd_t handle;
  int result;

  handle = grsd_init();
  fail_unless(handle != NULL);

  result = grsd_destroy(handle);
  fail_unless(result == 0);
}
END_TEST

START_TEST(init_failed) {
  grsd_t handle;

  ssh_proxy_env->ssh_bind_new_should_fail = 1;

  handle = grsd_init();
  fail_unless(handle == NULL);
}
END_TEST

Suite* libgrsd_suite() {
  Suite* s = suite_create("libgrsd_test");

  TCase* tc = tcase_create("libgrsd");
  tcase_add_checked_fixture(tc, setup, teardown);
  suite_add_tcase(s, tc);

  tcase_add_test(tc, init_destroy);
  tcase_add_test(tc, init_failed);

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

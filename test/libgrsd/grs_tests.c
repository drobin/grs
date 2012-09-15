#include <sys/errno.h>
#include <check.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "../../src/libgrsd/grs.h"

static grs_t handle;

static void setup() {
  fail_unless((handle = grs_init()) != NULL);
}

static void teardown() {
  fail_unless(grs_destroy(handle) == 0);
  handle = NULL;
}

START_TEST(destroy_null_handle) {
  fail_unless(grs_destroy(NULL) == -1);
}
END_TEST

START_TEST(get_process_env_null_handle) {
  fail_unless(grs_get_process_env(NULL) == NULL);
}
END_TEST

START_TEST(get_process_env) {
  fail_unless(grs_get_process_env(handle) != NULL);
}
END_TEST

TCase* grs_tcase() {
  TCase* tc = tcase_create("grs");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, destroy_null_handle);
  tcase_add_test(tc, get_process_env_null_handle);
  tcase_add_test(tc, get_process_env);

  return tc;
}

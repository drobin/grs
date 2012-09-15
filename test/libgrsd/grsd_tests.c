#include <sys/errno.h>
#include <check.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <libssh/libssh.h>

#include "../../src/libgrsd/grsd.h"

static grsd_t handle;

static void setup() {
  fail_unless((handle = grsd_init()) != NULL);
}

static void teardown() {
  fail_unless(grsd_destroy(handle) == 0);
  handle = NULL;
}

START_TEST(destroy_null_handle) {
  fail_unless(grsd_destroy(NULL) == -1);
}
END_TEST

TCase* grsd_tcase() {
  TCase* tc = tcase_create("grsd");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, destroy_null_handle);

  return tc;
}

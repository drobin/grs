#include <stdlib.h>
#include <unistd.h>

#include <check.h>

#include "../../src/libgrsd/grsd.h"
#include "../../src/libgrsd/handler.h"
#include "../../src/libgrsd/types.h"

static struct _grsd* handle;
static int fds[2];
static struct event_base* eb;
static struct event* ev;

static void setup() {
  fail_unless((handle = malloc(sizeof(struct _grsd))) != NULL);
  fail_unless(pipe(fds) == 0);
  fail_unless((eb = event_base_new()) != NULL);
  ev = event_new(eb, fds[0], EV_READ, grsd_handle_pipe, handle);
  fail_unless(ev != NULL);

  event_add(ev, NULL);
  handle->event_base = eb;
}

static void teardown() {
  event_free(ev);
  event_base_free(eb);
  close(fds[0]);
  close(fds[1]);
  free(handle);
}

START_TEST(quit_request) {
  fail_unless(write(fds[1], "q", 1) == 1);
  fail_unless(event_base_loop(eb, 0) == 0);
}
END_TEST

START_TEST(unknown_request) {
  fail_unless(write(fds[1], "x", 1) == 1);
  fail_unless(event_base_loop(eb, 0) == 1);
}
END_TEST

TCase* handle_pipe_tcase() {
  TCase* tc = tcase_create("handle_pipe");

  tcase_add_checked_fixture(tc, setup, teardown);
  tcase_add_test(tc, quit_request);
  tcase_add_test(tc, unknown_request);

  return tc;
}

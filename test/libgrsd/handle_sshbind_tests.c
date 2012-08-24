#include <stdlib.h>
#include <unistd.h>

#include <check.h>

#include "../../src/libgrsd/handler.h"
#include "../../src/libgrsd/types.h"
#include "../libssh_proxy.h"

static struct _grsd* handle;
static int fds[2];
static struct event_base* eb;
static struct event* ev;

static void setup() {
  fail_unless(libssh_proxy_init() == 0);
  fail_unless((handle = malloc(sizeof(struct _grsd))) != NULL);
  fail_unless(pipe(fds) == 0);
  fail_unless((eb = event_base_new()) != NULL);
  ev = event_new(eb, fds[0], EV_READ, grsd_handle_sshbind, handle);
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
  fail_unless(libssh_proxy_destroy() == 0);
}

START_TEST(session_create_failed) {
  // This let session_create() fail
  libssh_proxy_set_option_int("ssh_new", "fail", 1);

  event_active(ev, EV_READ, 1);
  fail_unless(event_base_loop(eb, 0) == 1);
}
END_TEST

START_TEST(session_accept_failed) {
  // This let session_accept() fail
  libssh_proxy_set_option_int("ssh_bind_accept", "fail", 1);

  event_active(ev, EV_READ, 1);
  fail_unless(event_base_loop(eb, 0) == 1);
}
END_TEST

START_TEST(bind_success) {
  event_active(ev, EV_READ, 1);
  fail_unless(event_base_loop(eb, EVLOOP_ONCE) == 0);
}
END_TEST

TCase* handle_sshbind_tcase() {
  TCase* tc = tcase_create("handle_sshbind");

  tcase_add_checked_fixture(tc, setup, teardown);
  tcase_add_test(tc, session_create_failed);
  tcase_add_test(tc, session_accept_failed);
  tcase_add_test(tc, bind_success);

  return tc;
}

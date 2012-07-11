#include <check.h>

#include <grsd.h>
#include <session.h>
#include <slist.h>

static slist_t slist;

static void setup() {
  fail_unless((slist = slist_init()) != NULL);
  fail_unless(slist_get_size(slist) == 0);
}

static void teardown() {
  fail_unless(slist_destroy(slist) == 0);
  slist = NULL;
}

START_TEST(destroy_null_handle) {
  fail_unless(slist_destroy(NULL) == -1);
}
END_TEST

START_TEST(get_size_null_slist) {
  fail_unless(slist_get_size(NULL) == -1);
}
END_TEST

START_TEST(prepend_null_slist) {
  grsd_t handle = grsd_init();
  session_t session = session_create(handle);
  
  fail_unless(slist_prepend(NULL, session));
  
  session_destroy(session);
  grsd_destroy(handle);
}
END_TEST

START_TEST(prepend_null_session) {
  fail_unless(slist_prepend(slist, NULL));
}
END_TEST

START_TEST(prepend_success) {
  grsd_t handle = grsd_init();
  session_t s1 = session_create(handle);
  session_t s2 = session_create(handle);
  session_t s3 = session_create(handle);

  fail_unless(slist_prepend(slist, s1) == 0);
  fail_unless(slist_get_size(slist) == 1);
  
  fail_unless(slist_prepend(slist, s2) == 0);
  fail_unless(slist_get_size(slist) == 2);
  
  fail_unless(slist_prepend(slist, s3) == 0);
  fail_unless(slist_get_size(slist) == 3);
  
  session_destroy(s1);
  session_destroy(s2);
  session_destroy(s3);
  grsd_destroy(handle);
}
END_TEST

START_TEST(remove_null_slist) {
  grsd_t handle = grsd_init();
  session_t session = session_create(handle);
  
  fail_unless(slist_remove(NULL, session));
  
  session_destroy(session);
  grsd_destroy(handle);
}
END_TEST

START_TEST(remove_null_session) {
  fail_unless(slist_remove(slist, NULL));
}
END_TEST

START_TEST(remove_success) {
  grsd_t handle = grsd_init();
  session_t s1 = session_create(handle);
  session_t s2 = session_create(handle);
  session_t s3 = session_create(handle);
  
  fail_unless(slist_prepend(slist, s1) == 0);
  fail_unless(slist_prepend(slist, s2) == 0);
  fail_unless(slist_prepend(slist, s3) == 0);
  fail_unless(slist_get_size(slist) == 3);
  
  fail_unless(slist_remove(slist, s2) == 0);
  fail_unless(slist_get_size(slist) == 2);

  fail_unless(slist_remove(slist, s1) == 0);
  fail_unless(slist_get_size(slist) == 1);

  fail_unless(slist_remove(slist, s3) == 0);
  fail_unless(slist_get_size(slist) == 0);
  
  session_destroy(s1);
  session_destroy(s2);
  session_destroy(s3);
  grsd_destroy(handle);
}
END_TEST

START_TEST(remove_not_found) {
  grsd_t handle = grsd_init();
  session_t s1 = session_create(handle);
  session_t s2 = session_create(handle);
  session_t s3 = session_create(handle);
  
  fail_unless(slist_prepend(slist, s1) == 0);
  fail_unless(slist_prepend(slist, s2) == 0);
  fail_unless(slist_get_size(slist) == 2);
  
  fail_unless(slist_remove(slist, s3) == -1);
  fail_unless(slist_get_size(slist) == 2);
  
  session_destroy(s1);
  session_destroy(s2);
  session_destroy(s3);
  grsd_destroy(handle);
}
END_TEST

START_TEST(clear_null_handle) {
  fail_unless(slist_clear(NULL) == -1);
}
END_TEST

START_TEST(clear_from_empty) {
  fail_unless(slist_clear(slist) == 0);
  fail_unless(slist_get_size(slist) == 0);
}
END_TEST

START_TEST(clear_from_non_empty) {
  grsd_t handle = grsd_init();
  session_t s1 = session_create(handle);
  session_t s2 = session_create(handle);
  session_t s3 = session_create(handle);
  
  fail_unless(slist_prepend(slist, s1) == 0);
  fail_unless(slist_prepend(slist, s2) == 0);
  fail_unless(slist_prepend(slist, s3) == 0);
  fail_unless(slist_get_size(slist) == 3);
  
  fail_unless(slist_clear(slist) == 0);
  fail_unless(slist_get_size(slist) == 0);
  
  session_destroy(s1);
  session_destroy(s2);
  session_destroy(s3);
  grsd_destroy(handle);
}
END_TEST

START_TEST(iterator_null_slist) {
  fail_unless(slist_iterator(NULL) == NULL);
}
END_TEST

START_TEST(it_destroy_null_it) {
  fail_unless(slist_iterator_destroy(NULL) == -1);
}
END_TEST

START_TEST(iterator_get_null_it) {
  fail_unless(slist_iterator_get(NULL) == NULL);
}
END_TEST

START_TEST(iterator_has_next_null_it) {
  fail_unless(slist_iterator_has_next(NULL) == -1);
}
END_TEST

START_TEST(iterator_next_null_it) {
  fail_unless(slist_iterator_next(NULL) == -1);
}
END_TEST

START_TEST(it_on_empty_list) {
  slist_it_t it = slist_iterator(slist);
  
  fail_unless(slist_iterator_get(it) == NULL);
  fail_unless(slist_iterator_next(it) == -1);
  fail_unless(slist_iterator_has_next(it) == 0);
  
  slist_iterator_destroy(it);
}
END_TEST

START_TEST(it_on_filled_list) {
  grsd_t handle = grsd_init();
  session_t s1 = session_create(handle);
  session_t s2 = session_create(handle);
  session_t s3 = session_create(handle);
  
  fail_unless(slist_prepend(slist, s1) == 0);
  fail_unless(slist_prepend(slist, s2) == 0);
  fail_unless(slist_prepend(slist, s3) == 0);
  fail_unless(slist_get_size(slist) == 3);
  
  slist_it_t it = slist_iterator(slist);
  
  fail_unless(slist_iterator_get(it) == s3);
  fail_unless(slist_iterator_has_next(it) == 1);
  fail_unless(slist_iterator_next(it) == 0);
  
  fail_unless(slist_iterator_get(it) == s2);
  fail_unless(slist_iterator_has_next(it) == 1);
  fail_unless(slist_iterator_next(it) == 0);
  
  fail_unless(slist_iterator_get(it) == s1);
  fail_unless(slist_iterator_has_next(it) == 0);
  fail_unless(slist_iterator_next(it) == -1);
  
  slist_iterator_destroy(it);
  
  session_destroy(s1);
  session_destroy(s2);
  session_destroy(s3);
  grsd_destroy(handle);
}
END_TEST

START_TEST(it_loop_on_filled_list) {
  grsd_t handle = grsd_init();
  session_t sessions[3];
  int i = 2;
  
  sessions[0] = session_create(handle);
  sessions[1] = session_create(handle);
  sessions[2] = session_create(handle);
  
  fail_unless(slist_prepend(slist, sessions[0]) == 0);
  fail_unless(slist_prepend(slist, sessions[1]) == 0);
  fail_unless(slist_prepend(slist, sessions[2]) == 0);
  fail_unless(slist_get_size(slist) == 3);
  
  slist_it_t it = slist_iterator(slist);
  
  while (slist_iterator_has_next(it)) {
    session_t session;
    
    fail_unless((session = slist_iterator_get(it)) != NULL);
    fail_unless(sessions[i] == session);
    fail_unless(slist_iterator_next(it) == 0);
    i--;
  }
  
  slist_iterator_destroy(it);
  
  session_destroy(sessions[0]);
  session_destroy(sessions[1]);
  session_destroy(sessions[2]);
  grsd_destroy(handle);
}
END_TEST

TCase* slist_tcase() {
  TCase* tc = tcase_create("slist");
  tcase_add_checked_fixture(tc, setup, teardown);
  
  tcase_add_test(tc, destroy_null_handle);
  tcase_add_test(tc, get_size_null_slist);
  tcase_add_test(tc, prepend_null_slist);
  tcase_add_test(tc, prepend_null_session);
  tcase_add_test(tc, prepend_success);
  tcase_add_test(tc, remove_null_slist);
  tcase_add_test(tc, remove_null_session);
  tcase_add_test(tc, remove_success);
  tcase_add_test(tc, remove_not_found);
  tcase_add_test(tc, clear_null_handle);
  tcase_add_test(tc, clear_from_empty);
  tcase_add_test(tc, clear_from_non_empty);
  tcase_add_test(tc, iterator_null_slist);
  tcase_add_test(tc, it_destroy_null_it);
  tcase_add_test(tc, iterator_get_null_it);
  tcase_add_test(tc, iterator_has_next_null_it);
  tcase_add_test(tc, iterator_next_null_it);
  tcase_add_test(tc, it_on_empty_list);
  tcase_add_test(tc, it_on_filled_list);
  tcase_add_test(tc, it_loop_on_filled_list);
      
  return tc;
}
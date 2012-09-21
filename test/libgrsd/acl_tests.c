/*******************************************************************************
 *
 * This file is part of grs.
 *
 * grs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * grs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with grs.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <check.h>

#include "../../src/libgrsd/acl.h"

/*
  root
   /\
  A  B
  /\ /\
 C D E F
*/
static const char* root_path[] = { NULL };
static const char* a_path[] = { "a", NULL };
static const char* b_path[] = { "b", NULL };
static const char* c_path[] = { "a", "c", NULL };
static const char* d_path[] = { "a", "d", NULL };
static const char* e_path[] = { "b", "e", NULL };
static const char* f_path[] = { "b", "f", NULL };

static acl_t acl;

static void setup() {
  fail_unless((acl = acl_init()) != NULL);
}

static void teardown() {
  fail_unless(acl_destroy(acl) == 0);
}

START_TEST(destroy_null_acl) {
  fail_unless(acl_destroy(NULL) == -1);
}
END_TEST

START_TEST(get_node_null_acl) {
  const char* path[] = { "foo", NULL };
  fail_unless(acl_get_node(NULL, path) == NULL);
}
END_TEST

START_TEST(get_node_null_path) {
  fail_unless(acl_get_node(acl, NULL) == NULL);
}
END_TEST

START_TEST(get_node_root) {
  acl_node_t root;

  fail_unless((root = acl_get_node(acl, root_path)) != NULL);
  fail_unless(acl_get_node(acl, root_path) == root);
  fail_unless(acl_node_get_name(root) == NULL);
}
END_TEST

START_TEST(get_node_a) {
  acl_node_t node;

  fail_unless((node = acl_get_node(acl, a_path)) != NULL);
  fail_unless(acl_get_node(acl, a_path) == node);
  fail_unless(strcmp(acl_node_get_name(node), "a") == 0);
}
END_TEST

START_TEST(get_node_b) {
  acl_node_t node;

  fail_unless((node = acl_get_node(acl, b_path)) != NULL);
  fail_unless(acl_get_node(acl, b_path) == node);
  fail_unless(strcmp(acl_node_get_name(node), "b") == 0);
}
END_TEST

START_TEST(get_node_c) {
  acl_node_t node;

  fail_unless((node = acl_get_node(acl, c_path)) != NULL);
  fail_unless(acl_get_node(acl, c_path) == node);
  fail_unless(strcmp(acl_node_get_name(node), "c") == 0);
}
END_TEST

START_TEST(get_node_d) {
  acl_node_t node;

  fail_unless((node = acl_get_node(acl, d_path)) != NULL);
  fail_unless(acl_get_node(acl, d_path) == node);
  fail_unless(strcmp(acl_node_get_name(node), "d") == 0);
}
END_TEST

START_TEST(get_node_e) {
  acl_node_t node;

  fail_unless((node = acl_get_node(acl, e_path)) != NULL);
  fail_unless(acl_get_node(acl, e_path) == node);
  fail_unless(strcmp(acl_node_get_name(node), "e") == 0);
}
END_TEST

START_TEST(get_node_f) {
  acl_node_t node;

  fail_unless((node = acl_get_node(acl, f_path)) != NULL);
  fail_unless(acl_get_node(acl, f_path) == node);
  fail_unless(strcmp(acl_node_get_name(node), "f") == 0);
}
END_TEST

START_TEST(node_get_name_null_node) {
  fail_unless(acl_node_get_name(NULL) == NULL);
}
END_TEST

TCase* acl_tcase() {
  TCase* tc = tcase_create("acl");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, destroy_null_acl);
  tcase_add_test(tc, get_node_null_acl);
  tcase_add_test(tc, get_node_null_path);
  tcase_add_test(tc, get_node_root);
  tcase_add_test(tc, get_node_a);
  tcase_add_test(tc, get_node_b);
  tcase_add_test(tc, get_node_c);
  tcase_add_test(tc, get_node_d);
  tcase_add_test(tc, get_node_e);
  tcase_add_test(tc, get_node_f);
  tcase_add_test(tc, node_get_name_null_node);

  return tc;
}

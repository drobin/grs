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
static const char* c_path[] = { "a", "c" };
static const char* d_path[] = { "a", "d" };
static const char* e_path[] = { "b", "e" };
static const char* f_path[] = { "b", "f" };

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

START_TEST(can_null_acl) {
  fail_unless(acl_can(NULL, c_path, 2) == -1);
}
END_TEST

START_TEST(can_null_path) {
  fail_unless(acl_can(acl, NULL, 2) == -1);
}
END_TEST

START_TEST(can_zero_len) {
  fail_unless(acl_can(acl, c_path, 0) == -1);
}
END_TEST

START_TEST(can_empty_tree_len_1) {
  acl_node_t root;
  struct acl_node_value* value;

  fail_unless(!acl_can(acl, c_path, 1));

  fail_unless((root = acl_get_node(acl, c_path, 0)) != NULL);
  fail_unless((value = acl_node_get_value(root, 1)) != NULL);
  value->flag = 1;

  fail_unless(acl_can(acl, c_path, 1));
}
END_TEST

START_TEST(can_empty_tree_len_2) {
  acl_node_t root;
  struct acl_node_value* value;

  fail_unless(!acl_can(acl, c_path, 2));

  fail_unless((root = acl_get_node(acl, c_path, 0)) != NULL);
  fail_unless((value = acl_node_get_value(root, 1)) != NULL);
  value->flag = 1;

  fail_unless(acl_can(acl, c_path, 2));
}
END_TEST

START_TEST(can_with_tree_len_shorter) {
  acl_node_t root, node;
  struct acl_node_value* value;

  fail_unless((node = acl_get_node(acl, c_path, 2)) != NULL);
  fail_unless(!acl_can(acl, c_path, 1));

  fail_unless((root = acl_get_node(acl, c_path, 0)) != NULL);
  fail_unless((value = acl_node_get_value(root, 1)) != NULL);
  value->flag = 1;

  fail_unless(acl_can(acl, c_path, 1));
}
END_TEST

START_TEST(can_with_tree_len_longer) {
  acl_node_t root, node;
  struct acl_node_value* value;

  fail_unless((node = acl_get_node(acl, c_path, 1)) != NULL);
  fail_unless(!acl_can(acl, c_path, 2));

  fail_unless((root = acl_get_node(acl, c_path, 0)) != NULL);
  fail_unless((value = acl_node_get_value(root, 1)) != NULL);
  value->flag = 1;

  fail_unless(acl_can(acl, c_path, 2));
}
END_TEST

START_TEST(can_with_tree_value_len_shorter) {
  acl_node_t node;
  struct acl_node_value* value;

  fail_unless((node = acl_get_node(acl, c_path, 2)) != NULL);
  fail_unless((value = acl_node_get_value(node, 1)) != NULL);
  value->flag = 1;

  fail_unless(!acl_can(acl, c_path, 1));

  value->flag = 0;

  fail_unless((node = acl_get_node(acl, c_path, 0)) != NULL);
  fail_unless((value = acl_node_get_value(node, 1)) != NULL);
  value->flag = 1;

  fail_unless(acl_can(acl, c_path, 1));
}
END_TEST

START_TEST(can_with_tree_value_len_longer) {
  acl_node_t node;
  struct acl_node_value* value;

  fail_unless((node = acl_get_node(acl, c_path, 1)) != NULL);
  fail_unless((value = acl_node_get_value(node, 1)) != NULL);
  value->flag = 1;

  fail_unless(acl_can(acl, c_path, 2));

  value->flag = 0;

  fail_unless((node = acl_get_node(acl, c_path, 0)) != NULL);
  fail_unless((value = acl_node_get_value(node, 1)) != NULL);
  value->flag = 1;

  fail_unless(!acl_can(acl, c_path, 2));
}
END_TEST

START_TEST(get_node_null_acl) {
  fail_unless(acl_get_node(NULL, c_path, 2) == NULL);
}
END_TEST

START_TEST(get_node_null_path) {
  fail_unless(acl_get_node(acl, NULL, 2) == NULL);
}
END_TEST

START_TEST(get_node_negative_len) {
  fail_unless(acl_get_node(acl, c_path, -1) == NULL);
}
END_TEST

START_TEST(get_node_root) {
  acl_node_t root;

  fail_unless((root = acl_get_node(acl, c_path, 0)) != NULL);
  fail_unless(acl_get_node(acl, c_path, 0) == root);
  fail_unless(acl_node_get_parent(root) == NULL);
  fail_unless(acl_node_get_name(root) == NULL);
}
END_TEST

START_TEST(get_node_a) {
  acl_node_t node, parent;

  fail_unless((node = acl_get_node(acl, c_path, 1)) != NULL);
  fail_unless((parent = acl_get_node(acl, c_path, 0)) != NULL);
  fail_unless(acl_get_node(acl, c_path, 1) == node);
  fail_unless(acl_node_get_parent(node) == parent);
  fail_unless(strcmp(acl_node_get_name(node), "a") == 0);
}
END_TEST

START_TEST(get_node_b) {
  acl_node_t node, parent;

  fail_unless((node = acl_get_node(acl, e_path, 1)) != NULL);
  fail_unless((parent = acl_get_node(acl, e_path, 0)) != NULL);
  fail_unless(acl_get_node(acl, e_path, 1) == node);
  fail_unless(acl_node_get_parent(node) == parent);
  fail_unless(strcmp(acl_node_get_name(node), "b") == 0);
}
END_TEST

START_TEST(get_node_c) {
  acl_node_t node, parent;

  fail_unless((node = acl_get_node(acl, c_path, 2)) != NULL);
  fail_unless((parent = acl_get_node(acl, c_path, 1)) != NULL);
  fail_unless(acl_get_node(acl, c_path, 2) == node);
  fail_unless(acl_node_get_parent(node) == parent);
  fail_unless(strcmp(acl_node_get_name(node), "c") == 0);
}
END_TEST

START_TEST(get_node_d) {
  acl_node_t node, parent;

  fail_unless((node = acl_get_node(acl, d_path, 2)) != NULL);
  fail_unless((parent = acl_get_node(acl, d_path, 1)) != NULL);
  fail_unless(acl_get_node(acl, d_path, 2) == node);
  fail_unless(acl_node_get_parent(node) == parent);
  fail_unless(strcmp(acl_node_get_name(node), "d") == 0);
}
END_TEST

START_TEST(get_node_e) {
  acl_node_t node, parent;

  fail_unless((node = acl_get_node(acl, e_path, 2)) != NULL);
  fail_unless((parent = acl_get_node(acl, e_path, 1)) != NULL);
  fail_unless(acl_get_node(acl, e_path, 2) == node);
  fail_unless(acl_node_get_parent(node) == parent);
  fail_unless(strcmp(acl_node_get_name(node), "e") == 0);
}
END_TEST

START_TEST(get_node_f) {
  acl_node_t node, parent;

  fail_unless((node = acl_get_node(acl, f_path, 2)) != NULL);
  fail_unless((parent = acl_get_node(acl, f_path, 1)) != NULL);
  fail_unless(acl_get_node(acl, f_path, 2) == node);
  fail_unless(acl_node_get_parent(node) == parent);
  fail_unless(strcmp(acl_node_get_name(node), "f") == 0);
}
END_TEST

START_TEST(node_get_parent_null_node) {
  fail_unless(acl_node_get_parent(NULL) == NULL);
}
END_TEST

START_TEST(node_get_name_null_node) {
  fail_unless(acl_node_get_name(NULL) == NULL);
}
END_TEST

START_TEST(node_get_value_null_node) {
  fail_unless(acl_node_get_value(NULL, 0) == NULL);
}
END_TEST

START_TEST(node_get_value_no_create) {
  acl_node_t node;

  fail_unless((node = acl_get_node(acl, c_path, 2)) != NULL);
  fail_unless(acl_node_get_value(node, 0) == NULL);
}
END_TEST

START_TEST(node_get_value_create) {
  acl_node_t node;
  struct acl_node_value* value;

  fail_unless((node = acl_get_node(acl, c_path, 2)) != NULL);
  fail_unless((value = acl_node_get_value(node, 1)) != NULL);
  fail_unless(acl_node_get_value(node, 0) == value);
  fail_unless(acl_node_get_value(node, 1) == value);
}
END_TEST

TCase* acl_tcase() {
  TCase* tc = tcase_create("acl");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, destroy_null_acl);
  tcase_add_test(tc, can_null_acl);
  tcase_add_test(tc, can_null_path);
  tcase_add_test(tc, can_zero_len);
  tcase_add_test(tc, can_empty_tree_len_1);
  tcase_add_test(tc, can_empty_tree_len_2);
  tcase_add_test(tc, can_with_tree_len_shorter);
  tcase_add_test(tc, can_with_tree_len_longer);
  tcase_add_test(tc, can_with_tree_value_len_shorter);
  tcase_add_test(tc, can_with_tree_value_len_longer);
  tcase_add_test(tc, get_node_null_acl);
  tcase_add_test(tc, get_node_null_path);
  tcase_add_test(tc, get_node_negative_len);
  tcase_add_test(tc, get_node_root);
  tcase_add_test(tc, get_node_a);
  tcase_add_test(tc, get_node_b);
  tcase_add_test(tc, get_node_c);
  tcase_add_test(tc, get_node_d);
  tcase_add_test(tc, get_node_e);
  tcase_add_test(tc, get_node_f);
  tcase_add_test(tc, node_get_name_null_node);
  tcase_add_test(tc, node_get_parent_null_node);
  tcase_add_test(tc, node_get_value_null_node);
  tcase_add_test(tc, node_get_value_no_create);
  tcase_add_test(tc, node_get_value_create);

  return tc;
}

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

TCase* acl_tcase() {
  TCase* tc = tcase_create("acl");
  tcase_add_checked_fixture(tc, setup, teardown);

  tcase_add_test(tc, destroy_null_acl);

  return tc;
}

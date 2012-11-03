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

#include "../../src/extension/git/capabilities.h"

START_TEST(null_str) {
  fail_unless(capabilities_parse(NULL, 5) == -1);
}
END_TEST

START_TEST(empty_str) {
  fail_unless(capabilities_parse("", 0) == 0);
}
END_TEST

START_TEST(space_str) {
  fail_unless(capabilities_parse("   ", 3) == 0);
}
END_TEST

START_TEST(one_capability) {
  fail_unless(capabilities_parse("multi_ack", 9) == multi_ack);
}
END_TEST

START_TEST(two_capabilities) {
  int result;

  result = capabilities_parse("multi_ack multi_ack_detailed", 28);
  fail_unless(result & multi_ack);
  fail_unless(result & multi_ack_detailed);
}
END_TEST

START_TEST(unknown_capability) {
  fail_unless(capabilities_parse("foo", 3) == -1);
}
END_TEST

START_TEST(parse_multi_ack) {
  fail_unless(capabilities_parse("multi_ack", 9) == multi_ack);
}
END_TEST

START_TEST(parse_multi_ack_detailed) {
  fail_unless(capabilities_parse("multi_ack_detailed", 18) == multi_ack_detailed);
}
END_TEST

START_TEST(parse_thin_pack) {
  fail_unless(capabilities_parse("thin-pack", 9) == thin_pack);
}
END_TEST

START_TEST(parse_side_band) {
  fail_unless(capabilities_parse("side-band", 9) == side_band);
}
END_TEST

START_TEST(parse_side_band_64k) {
  fail_unless(capabilities_parse("side-band-64k", 13) == side_band_64k);
}
END_TEST

START_TEST(parse_ofs_delta) {
  fail_unless(capabilities_parse("ofs-delta", 9) == ofs_delta);
}
END_TEST

START_TEST(parse_shallow) {
  fail_unless(capabilities_parse("shallow", 7) == shallow);
}
END_TEST

START_TEST(parse_no_progress) {
  fail_unless(capabilities_parse("no-progress", 11) == no_progress);
}
END_TEST

START_TEST(parse_include_tag) {
  fail_unless(capabilities_parse("include-tag", 11) == include_tag);
}
END_TEST

START_TEST(parse_report_status) {
  fail_unless(capabilities_parse("report-status", 13) == report_status);
}
END_TEST

START_TEST(parse_delete_refs) {
  fail_unless(capabilities_parse("delete-refs", 11) == delete_refs);
}
END_TEST

TCase* capabilities_tcase() {
  TCase* tc = tcase_create("capabilities");

  tcase_add_test(tc, null_str);
  tcase_add_test(tc, empty_str);
  tcase_add_test(tc, space_str);
  tcase_add_test(tc, one_capability);
  tcase_add_test(tc, two_capabilities);
  tcase_add_test(tc, unknown_capability);
  tcase_add_test(tc, parse_multi_ack);
  tcase_add_test(tc, parse_multi_ack_detailed);
  tcase_add_test(tc, parse_thin_pack);
  tcase_add_test(tc, parse_side_band);
  tcase_add_test(tc, parse_side_band_64k);
  tcase_add_test(tc, parse_ofs_delta);
  tcase_add_test(tc, parse_shallow);
  tcase_add_test(tc, parse_no_progress);
  tcase_add_test(tc, parse_include_tag);
  tcase_add_test(tc, parse_report_status);
  tcase_add_test(tc, parse_delete_refs);

  return tc;
}

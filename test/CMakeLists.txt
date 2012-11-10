################################################################################
#
# This file is part of grs.
#
# grs is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# grs is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with grs.  If not, see <http://www.gnu.org/licenses/>.
#
################################################################################

SET(GRS_TEST_SOURCES
  ext_core/suite.c
  ext_core/tests.c
  ext_git/capabilities_tests.c
  ext_git/checksum_tests.c
  ext_git/compress_tests.c
  ext_git/extension_tests.c
  ext_git/packfile_negotiation_tests.c
  ext_git/packfile_transfer_tests.c
  ext_git/pkt_line_tests.c
  ext_git/reference_discovery_tests.c
  ext_git/suite.c
  libgrs/acl_tests.c
  libgrs/binbuf_tests.c
  libgrs/buffer_tests.c
  libgrs/grs_tests.c
  libgrs/session_tests.c
  libgrs/suite.c
  grs_test.c
)

set(INTEGRATION_TESTS "${CMAKE_CURRENT_SOURCE_DIR}/integration")

add_executable(grs_test ${GRS_TEST_SOURCES})
add_test(grs_test grs_test)
add_test(grs_info ${INTEGRATION_TESTS}/grs_info.rb)
add_test(ls_remote ${INTEGRATION_TESTS}/ls_remote.rb)
add_test(clone ${INTEGRATION_TESTS}/clone.rb)
add_test(pull ${INTEGRATION_TESTS}/pull.rb)

include_directories(
  ${LIBEVENT_INCLUDE_DIR}
  ${LIBSSH_INCLUDE_DIR}
  ${CHECK_INCLUDE_DIR}
  ${LIBGRS_INCLUDE_DIR}
)

target_link_libraries(grs_test
  libgrs libext_core libext_git
  ${ZLIB_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${LIBGIT2_LIBRARY}
  ${CHECK_LIBRARY}
)

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
  ext_git/extension.c
  ext_git/protocol_tests.c
  ext_git/suite.c
  libgrs/acl_tests.c
  libgrs/buffer_tests.c
  libgrs/grs_tests.c
  libgrs/session_tests.c
  libgrs/suite.c
  grs_test.c
)

add_executable(grs_test ${GRS_TEST_SOURCES})
add_test(grs_test grs_test)

include_directories(
  ${LIBEVENT_INCLUDE_DIR}
  ${LIBSSH_INCLUDE_DIR}
  ${CHECK_INCLUDE_DIR}
  ${LIBGRS_INCLUDE_DIR}
)

target_link_libraries(grs_test
  libgrs libext_core libext_git
  ${LIBEVENT_LIBRARY}
  ${CHECK_LIBRARY}
  pthread
)

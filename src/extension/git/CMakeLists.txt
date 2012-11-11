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

SET(LIBEXT_GIT_SOURCES
  capabilities.c
  checksum.c
  compress.c
  git.c
  libgit2.c
  packfile_negotiation.c
  packfile_transfer.c
  pkt_line.c
  reference_discovery.c
  report_status.c
  update_request.c
)

include_directories(
  ${LIBGRS_INCLUDE_DIR}
  ${LIBGIT2_INCLUDE_DIR}
)

add_library(libext_git STATIC ${LIBEXT_GIT_SOURCES})

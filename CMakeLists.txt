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

cmake_minimum_required(VERSION 2.8)
project(grs)

# where to look first for custom cmake modules
set(CMAKE_MODULE_PATH
  ${CMAKE_SOURCE_DIR}/cmake/modules
)

find_package(libssh)
find_package(libgit2)
find_package(ZLIB)
find_package(OpenSSL)
find_package(check)

if(NOT LIBSSH_FOUND)
  message(FATAL_ERROR "Could not find libssh")
endif(NOT LIBSSH_FOUND)

if(NOT LIBGIT2_FOUND)
  message(FATAL_ERROR "Could not find libgit2")
endif(NOT LIBGIT2_FOUND)

if(NOT ZLIB_FOUND)
  message(FATAL_ERROR "Could not find zlib")
endif(NOT ZLIB_FOUND)

if(NOT OPENSSL_FOUND)
  message(FATAL_ERROR "Could not find OpenSSL")
endif(NOT OPENSSL_FOUND)

if(NOT CHECK_FOUND)
  message(FATAL_ERROR "Could not find check")
endif(NOT CHECK_FOUND)

add_definitions(-Wall -Werror)
enable_testing()

# Debug-mode: enable debug-symbols for the compiler and create a
#             ENABLE_DEBUG-macro. So the application can react on it.
if(ENABLE_DEBUG)
  add_definitions(-g -O0 -DENABLE_DEBUG)
endif(ENABLE_DEBUG)

# define some useful helper-variables
set(LIBGRS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src)

add_subdirectory(src)
add_subdirectory(test)

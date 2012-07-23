cmake_minimum_required(VERSION 2.8)
project(grs)

# where to look first for custom cmake modules
set(CMAKE_MODULE_PATH
  ${CMAKE_SOURCE_DIR}/cmake/modules
)

find_package(libssh)
find_package(libevent)
find_package(check)

if(NOT LIBSSH_FOUND)
  message(FATAL_ERROR "Could not find libssh")
endif(NOT LIBSSH_FOUND)

if(NOT LIBEVENT_FOUND)
  message(FATAL_ERROR "Could not find libevent")
endif(NOT LIBEVENT_FOUND)

if(NOT CHECK_FOUND)
  message(FATAL_ERROR "Could not find check")
endif(NOT CHECK_FOUND)

add_definitions(-Wall -Werror)
enable_testing()

# define some useful helper-variables
set(LIBGRSD_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/libgrsd)

add_subdirectory(src)
add_subdirectory(test)

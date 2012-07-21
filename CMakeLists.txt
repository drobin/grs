cmake_minimum_required(VERSION 2.8)
project(grs)

# where to look first for custom cmake modules
set(CMAKE_MODULE_PATH
  ${CMAKE_SOURCE_DIR}/cmake/modules
)

find_package(libssh)

if(NOT LIBSSH_FOUND)
  message(FATAL_ERROR "Could not find libssh")
endif(NOT LIBSSH_FOUND)

add_definitions(-Wall -Werror)
enable_testing()

add_subdirectory(src)
add_subdirectory(test)

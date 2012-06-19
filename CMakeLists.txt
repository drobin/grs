cmake_minimum_required(VERSION 2.8)
project(grs)

add_definitions(-Wall -Werror)
enable_testing()

add_subdirectory(src)
add_subdirectory(test)

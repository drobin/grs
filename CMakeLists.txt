cmake_minimum_required(VERSION 2.8)
project(grs)

ADD_DEFINITIONS(-Wall -Werror)

set(GRSD_SOURCES
  src/grsd/main.c
  src/grsd/grsd.c
)

add_executable(grsd ${GRSD_SOURCES})

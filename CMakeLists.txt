cmake_minimum_required(VERSION 2.8)
project(grs)

ADD_DEFINITIONS(-Wall -Werror)

set(GRSD_SOURCES
  grsd/main.c
  grsd/grsd.c
)

add_executable(grsd ${GRSD_SOURCES})

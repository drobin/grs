SET(LIBGRSD_SOURCES
  grsd.c
)

add_library(libgrsd STATIC ${LIBGRSD_SOURCES})
include_directories(${CMAKE_SOURCE_DIR}/../libssh/include)

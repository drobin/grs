SET(LIBGRSD_SOURCES
  grsd.c
  session.c
)

add_library(libgrsd STATIC ${LIBGRSD_SOURCES})
include_directories(${CMAKE_SOURCE_DIR}/../libssh/include)

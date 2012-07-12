SET(LIBGRSD_SOURCES
  grsd.c
  handler_pipe.c
  log.c
  session.c
  slist.c
)

add_library(libgrsd STATIC ${LIBGRSD_SOURCES})
include_directories(${CMAKE_SOURCE_DIR}/../libssh/include /opt/local/include)

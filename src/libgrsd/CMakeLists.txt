SET(LIBGRSD_SOURCES
  grsd.c
  log.c
  process.c
  session.c
  session2.c
  slist.c
)

add_library(libgrsd STATIC ${LIBGRSD_SOURCES})
include_directories(
  ${LIBSSH_INCLUDE_DIR}
)

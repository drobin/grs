SET(LIBGRSD_SOURCES
  grsd.c
  log.c
  process.c
  session2.c
)

add_library(libgrsd STATIC ${LIBGRSD_SOURCES})
include_directories(
  ${LIBSSH_INCLUDE_DIR}
)

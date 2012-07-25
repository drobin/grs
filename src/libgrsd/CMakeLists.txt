SET(LIBGRSD_SOURCES
  grsd.c
  handler_pipe.c
  handler_session.c
  handler_sshbind.c
  log.c
  process.c
  session.c
  slist.c
)

add_library(libgrsd STATIC ${LIBGRSD_SOURCES})
include_directories(
  ${LIBSSH_INCLUDE_DIR}
  ${LIBEVENT_INCLUDE_DIR}
)

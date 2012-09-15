SET(LIBGRSD_SOURCES
  grsd.c
  log.c
  process.c
  session.c
)

add_library(libgrsd STATIC ${LIBGRSD_SOURCES})

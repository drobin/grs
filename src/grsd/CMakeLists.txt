set(GRSD_SOURCES
  main.c
)

add_executable(grsd ${GRSD_SOURCES})

include_directories(
  ${LIBEVENT_INCLUDE_DIR}
  ${LIBGRSD_INCLUDE_DIR}
)
target_link_libraries(grsd
  libgrsd
  ${LIBSSH_LIBRARY}
  ${LIBEVENT_LIBRARY}
)

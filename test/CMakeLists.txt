SET(GRS_TEST_SOURCES
  hostkey.c
  libssh.c
  suite.c
  libgrsd/grsd_tests.c
  libgrsd/process_tests.c
  libgrsd/session_tests.c
  libgrsd/slist_tests.c
)

add_executable(grs_test ${GRS_TEST_SOURCES})
add_test(grs_test grs_test)

include_directories(
  ${LIBGRSD_INCLUDE_DIR}
  ${LIBSSH_INCLUDE_DIR}
  ${CHECK_INCLUDE_DIR}
)

target_link_libraries(grs_test
  libgrsd
  ${LIBEVENT_LIBRARY}
  ${CHECK_LIBRARY}
  pthread
)

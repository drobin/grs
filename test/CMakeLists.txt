SET(GRS_TEST_SOURCES
  hostkey.c
  suite.c
  libgrsd/grsd_tests.c
  libgrsd/session_tests.c
  libgrsd/slist_tests.c
)

include_directories(${CMAKE_SOURCE_DIR}/../bin/include)
include_directories(${CMAKE_SOURCE_DIR}/src/libgrsd)
link_directories(${CMAKE_SOURCE_DIR}/../bin/lib /opt/local/lib)

add_executable(grs_test ${GRS_TEST_SOURCES})
target_link_libraries(grs_test
  libgrsd
  ${LIBSSH_LIBRARY}
  ${LIBEVENT_LIBRARY}
  check
)

add_test(grs_test grs_test)

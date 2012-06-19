SET(LIBGRSD_TEST_SOURCES
  suite.c
)

add_executable(libgrsd_test ${LIBGRSD_TEST_SOURCES})
add_test(libgrsd libgrsd_test)

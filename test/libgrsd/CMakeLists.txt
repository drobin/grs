SET(LIBGRSD_TEST_SOURCES
  suite.c
)

link_directories(${CMAKE_SOURCE_DIR}/../bin/lib)
include_directories(${CMAKE_SOURCE_DIR}/../bin/include)

add_executable(libgrsd_test ${LIBGRSD_TEST_SOURCES})
target_link_libraries(libgrsd_test check)

add_test(libgrsd libgrsd_test)

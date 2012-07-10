SET(GRS_TEST_SOURCES
  hostkey.c
  suite.c
  libgrsd/grsd_tests.c
  libgrsd/session_tests.c
)

include_directories(${CMAKE_SOURCE_DIR}/../bin/include)
include_directories(${CMAKE_SOURCE_DIR}/src/libgrsd)
link_directories(${CMAKE_SOURCE_DIR}/../bin/lib)

add_executable(grs_test ${GRS_TEST_SOURCES})
target_link_libraries(grs_test libgrsd ssh check)

add_test(grs_test grs_test)

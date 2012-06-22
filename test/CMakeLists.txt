SET(GRS_TEST_SOURCES
  suite.c
  libgrsd/libssh_proxy.c
  libgrsd/libgrsd_tests.c
)

include_directories(${CMAKE_SOURCE_DIR}/../bin/include)
include_directories(${CMAKE_SOURCE_DIR}/src/libgrsd)
link_directories(${CMAKE_SOURCE_DIR}/../bin/lib)

add_executable(grs_test ${GRS_TEST_SOURCES})
target_link_libraries(grs_test libgrsd check)

add_test(grs_test grs_test)

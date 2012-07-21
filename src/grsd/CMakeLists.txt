set(GRSD_SOURCES
  main.c
)

link_directories(${CMAKE_SOURCE_DIR}/../bin/lib /opt/local/lib)

add_executable(grsd ${GRSD_SOURCES})

include_directories(${PROJECT_SOURCE_DIR}/src/libgrsd)
target_link_libraries(grsd libgrsd ${LIBSSH_LIBRARY} event)

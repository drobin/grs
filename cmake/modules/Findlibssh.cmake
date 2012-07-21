find_path(LIBSSH_INCLUDE_DIR
  NAMES
    libssh/libssh.h
  PATHS
    ${_LIBSSH_DIR}/include
    /usr/include
    /usr/local/include
    /opt/local/include
)


find_library(LIBSSH_LIBRARY
  NAMES
    ssh
  PATHS
    ${_LIBSSH_DIR}/lib
    /usr/lib
    /usr/local/lib
    /opt/local/lib
)

if(LIBSSH_INCLUDE_DIR)
  message(STATUS "looking for libssh includes - ${LIBSSH_INCLUDE_DIR}")
else()
  message(STATUS "looking for libssh includes - not found")
endif()

if(LIBSSH_LIBRARY)
  message(STATUS "looking for libssh libs - ${LIBSSH_LIBRARY}")
else()
  message(STATUS "looking for libssh libs - not found")
endif()

if(LIBSSH_INCLUDE_DIR AND LIBSSH_LIBRARY)
  set(LIBSSH_FOUND TRUE)
else()
  set(LIBSSH_FOUND FALSE)
endif()

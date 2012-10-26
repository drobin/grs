find_path(
  LIBSSH_INCLUDE_DIR
  libssh/libssh.h
)

find_library(
  LIBSSH_LIBRARY
  ssh
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

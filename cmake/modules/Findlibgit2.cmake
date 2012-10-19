find_path(LIBGIT2_INCLUDE_DIR
  NAMES
    git2.h
  PATHS
    ${_LIBGIT2_DIR}/include
    /usr/include
    /usr/local/include
    /opt/local/include
)

find_library(LIBGIT2_LIBRARY
  NAMES
    git2
  PATHS
    ${_LIBGIT2_DIR}/lib
    /usr/lib
    /usr/local/lib
    /opt/local/lib
)

if(LIBGIT2_INCLUDE_DIR)
  message(STATUS "looking for libgit2 includes - ${LIBGIT2_INCLUDE_DIR}")
else()
  message(STATUS "looking for libgit2 includes - not found")
endif()

if(LIBGIT2_LIBRARY)
  message(STATUS "looking for libgit2 libs - ${LIBGIT2_LIBRARY}")
else()
  message(STATUS "looking for libgit2 libs - not found")
endif()

if(LIBGIT2_INCLUDE_DIR AND LIBGIT2_LIBRARY)
  set(LIBGIT2_FOUND TRUE)
else()
  set(LIBGIT2_FOUND FALSE)
endif()

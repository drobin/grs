find_path(
  LIBGIT2_INCLUDE_DIR
  git2.h
)

find_library(
  LIBGIT2_LIBRARY
  git2
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

find_path(
  CHECK_INCLUDE_DIR
  check.h
)

find_library(
  CHECK_LIBRARY
  check
)

if(CHECK_INCLUDE_DIR)
  message(STATUS "looking for check includes - ${CHECK_INCLUDE_DIR}")
else()
  message(STATUS "looking for check includes - not found")
endif()

if(CHECK_LIBRARY)
  message(STATUS "looking for check libs - ${CHECK_LIBRARY}")
else()
  message(STATUS "looking for check libs - not found")
endif()

if(CHECK_INCLUDE_DIR AND CHECK_LIBRARY)
  set(CHECK_FOUND TRUE)
else()
  set(CHECK_FOUND FALSE)
endif()

find_path(CHECK_INCLUDE_DIR
  NAMES
    check.h
  PATHS
    ${_CHECK_DIR}/include
    /usr/include
    /usr/local/include
    /opt/local/include
)

find_library(CHECK_LIBRARY
  NAMES
    check
  PATHS
    ${_CHECK_DIR}/lib
    /usr/lib
    /usr/local/lib
    /opt/local/lib
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

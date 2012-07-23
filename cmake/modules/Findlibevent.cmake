find_path(LIBEVENT_INCLUDE_DIR
  NAMES
    event2/event.h
    event.h
  PATHS
    ${_LIBEVENT_DIR}/include
    /usr/include
    /usr/local/include
    /opt/local/include
)

find_library(LIBEVENT_LIBRARY
  NAMES
    event
  PATHS
    ${_LIBEVENT_DIR}/lib
    /usr/lib
    /usr/local/lib
    /opt/local/lib
)

if(LIBEVENT_INCLUDE_DIR)
  message(STATUS "looking for libevent includes - ${LIBEVENT_INCLUDE_DIR}")
else()
  message(STATUS "looking for libevent includes - not found")
endif()

if(LIBEVENT_LIBRARY)
  message(STATUS "looking for libevent libs - ${LIBEVENT_LIBRARY}")
else()
  message(STATUS "looking for libevent libs - not found")
endif()

if(LIBEVENT_INCLUDE_DIR AND LIBEVENT_LIBRARY)
  set(LIBEVENT_FOUND TRUE)
else()
  set(LIBEVENT_FOUND FALSE)
endif()

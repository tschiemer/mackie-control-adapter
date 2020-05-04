cmake_minimum_required(VERSION 3.3)
project(rme-totalmix-midi-adapter)


set(RTMIDI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/rtmidi)
set(MIDIMESSAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/midimessage)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

link_directories("/usr/local/lib/")

add_subdirectory(${MIDIMESSAGE_DIR} EXCLUDE_FROM_ALL)
set(MIDIMESSAGE_INCLUDES ${MIDIMESSAGE_DIR}/include)

add_subdirectory(${RTMIDI_DIR} EXCLUDE_FROM_ALL)
set(RTMIDI_INCLUDES ${RTMIDI_DIR})


include_directories(
        "include"
        "${MIDIMESSAGE_INCLUDES}"
        "${RTMIDI_INCLUDES}"
)

file(GLOB GENERAL_INC
    "include/*.h"
    "include/*.hpp"
)

file(GLOB CONTROL_SURFACES_INC
    "include/*.h"
    "include/*.hpp"
    "include/ControlSurface/*.h"
    "include/ControlSurface/*.hpp"
)

file(GLOB GENERAL_SRC
    "src/*.c"
    "src/*.cpp"
)

file(GLOB CONTROL_SURFACES_SRC
    "src/ControlSurface/*.c"
    "src/ControlSurface/*.cpp"
)

set(HEADER_FILES
    ${GENERAL_INC}
    ${CONTROL_SURFACES_INC}
)

set(SOURCE_FILES
  ${GENERAL_SRC}
  ${CONTROL_SURFACES_SRC}
)


add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} midimessage rtmidi)

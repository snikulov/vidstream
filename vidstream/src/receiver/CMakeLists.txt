cmake_minimum_required(VERSION 2.8)
set(TARGET_NAME receiver)
project(${TARGET_NAME} CXX)

include_directories(hdr)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SRC_FILES
    src/main.cpp
)

add_executable(${TARGET_NAME} ${SRC_FILES})

target_link_libraries(${TARGET_NAME}
    ${OpenCV_LIBS} ${Boost_LIBRARIES}
    split
    nanomsg
    )
if(UNIX)
    target_link_libraries(${TARGET_NAME}
        bch
    )
endif()

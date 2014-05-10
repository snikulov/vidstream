cmake_minimum_required(VERSION 2.8)

project(capture CXX)

include_directories(hdr)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SRC_FILES
    src/main_capture.cpp
)

add_executable(capture ${SRC_FILES})
target_link_libraries(capture ${OpenCV_LIBS} ${Boost_LIBRARIES})

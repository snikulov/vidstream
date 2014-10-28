cmake_minimum_required(VERSION 2.8)

project(test_split CXX)

include_directories(${OpenCV_INCLUDE_DIRS})

set(SRC_FILES
    test_split.cpp
)

add_executable(test_split ${SRC_FILES})
target_link_libraries(test_split
    split
    corrupt
    ${Boost_LIBRARIES}
    ${OpenCV_LIBS}
    itpp
    )

add_test(NAME test_split COMMAND test_split --log_level=message ${CMAKE_SOURCE_DIR}/test/data/img_test_split.jpg)


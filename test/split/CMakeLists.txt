cmake_minimum_required(VERSION 3.10)

project(test_split)

include_directories(${OpenCV_INCLUDE_DIRS})

set(SRC_FILES
    test_split.cpp
)

add_executable(test_split ${SRC_FILES})
target_link_libraries(test_split
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
    split
    corrupt
    channel
    )
if(WITH_ITPP)
  target_link_libraries(test_split
    ${ITPP_LIBRARIES}
    )
endif()

add_test(NAME test_split COMMAND test_split --log_level=message ${CMAKE_SOURCE_DIR}/test/data/img_test_split.jpg)

cmake_minimum_required(VERSION 2.8)

set(TEST_NAME test_channel)
project(${TEST_NAME} CXX)

include_directories(${OpenCV_INCLUDE_DIRS})

set(SRC_FILES
    test_channel.cpp
)

add_definitions(-DNOMINMAX)

add_executable(${TEST_NAME} ${SRC_FILES})
target_link_libraries(${TEST_NAME}
    ${Boost_LIBRARIES}
    ${OpenCV_LIBS}
    ${LOG4CPLUS_LIBRARIES}
    ${ITPP_LIBRARIES}
    split
    channel
    nanomsg
    utils
    bch
    )

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} --log_level=message ${CMAKE_SOURCE_DIR}/test/data/img_test_split.jpg)


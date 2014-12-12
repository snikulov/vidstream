cmake_minimum_required(VERSION 2.8)

set(TEST_NAME test_bch_codec)
project(${TEST_NAME} CXX)

set(SRC_FILES
    test_bch_codec.cpp
)

add_executable(${TEST_NAME} ${SRC_FILES})
target_link_libraries(${TEST_NAME}
    ${Boost_LIBRARIES}
    ${ITPP_LIBRARIES}
    ${LOG4CPLUS_LIBRARIES}
    bch
    )

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} --log_level=message ${CMAKE_SOURCE_DIR}/test/data/img_test_split.jpg)


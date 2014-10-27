cmake_minimum_required(VERSION 2.8)
set(TARGET_NAME channel)
project(${TARGET_NAME} CXX)

include_directories(hdr)

set(HDR_FILES hdr/channel_int.hpp hdr/transport_int.hpp hdr/codec_int.hpp)
set(SRC_FILES src/channel.cpp)

add_definitions(-DNOMINMAX)

add_library(${TARGET_NAME} ${SRC_FILES} ${HDR_FILES})
target_link_libraries(${TEST_NAME}
    itpp
    )



cmake_minimum_required(VERSION 2.8)
set(TARGET_NAME channel)
project(${TARGET_NAME} CXX)

include_directories(hdr)

set(HDR_FILES
    hdr/channel_int.hpp
    hdr/out_channel_int.hpp
    hdr/in_channel_int.hpp
    hdr/transport_int.hpp
    hdr/bchwrapper_int.hpp
    hdr/codec_int.hpp
    hdr/bch_codec_itpp.hpp
    hdr/bch_codec_kernel.hpp
    hdr/bch_codec_none.hpp
)

set(SRC_FILES
#    src/channel.cpp
    src/jpeg_stream_parser.cpp
    src/in_channel.cpp
    src/out_channel.cpp)

add_definitions(-DNOMINMAX)

add_library(${TARGET_NAME}
    ${SRC_FILES}
    ${HDR_FILES})

lint_check_sources(${TARGET_NAME} ${SRC_FILES})


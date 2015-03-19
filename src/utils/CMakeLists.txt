cmake_minimum_required(VERSION 2.8)
set(TARGET_NAME utils)
project(${TARGET_NAME} CXX)

include_directories(hdr)

set(HDR_FILES
    hdr/converters_int.hpp)
set(SRC_FILES
    src/converters.cpp
)

add_library(${TARGET_NAME} STATIC ${SRC_FILES} ${HDR_FILES})

lint_check_sources(${TARGET_NAME} ${SRC_FILES})


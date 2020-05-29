cmake_minimum_required(VERSION 2.8)
set(TARGET_NAME split)
project(${TARGET_NAME})

include_directories(hdr)

set(HDR_FILES
    hdr/split_int.h)
set(SRC_FILES
    src/split.cpp
)

add_library(${TARGET_NAME} ${SRC_FILES} ${HDR_FILES})

lint_check_sources(${TARGET_NAME} ${SRC_FILES})

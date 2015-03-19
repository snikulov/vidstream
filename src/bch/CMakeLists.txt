cmake_minimum_required(VERSION 2.8)
set(TARGET_NAME bch)
project(${TARGET_NAME} CXX)

include_directories(hdr)

set(HDR_FILES
    hdr/bch.h
    )
set(SRC_FILES
    src/bch.cpp
    src/ecc.cpp
    )

add_library(${TARGET_NAME} ${SRC_FILES} ${HDR_FILES})

lint_check_sources(${TARGET_NAME} ${SRC_FILES})


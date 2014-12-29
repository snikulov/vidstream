cmake_minimum_required(VERSION 2.8)
set(TARGET_NAME corrupt)
project(${TARGET_NAME} CXX)

include_directories(hdr)

set(HDR_FILES
    hdr/corrupt_int.h
)
set(SRC_FILES
    src/corrupt.cpp 
)

add_library(${TARGET_NAME} ${SRC_FILES} ${HDR_FILES} ${GLB_HDR_FILES})

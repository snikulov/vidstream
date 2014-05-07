cmake_minimum_required(VERSION 2.8)

project(split CXX)

include_directories(hdr)

set(SRC_FILES
    src/split.cpp
)

add_library(split ${SRC_FILES})

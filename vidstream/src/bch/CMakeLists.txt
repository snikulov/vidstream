cmake_minimum_required(VERSION 2.8)

project(bch CXX)

include_directories(hdr)

set(SRC_FILES
    src/bch.cpp
    src/ecc.cpp
)

add_library(bch ${SRC_FILES})

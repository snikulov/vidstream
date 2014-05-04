cmake_minimum_required(VERSION 2.8)

project(vidstream C CXX)

find_package(Boost REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories(include)

add_subdirectory(src)

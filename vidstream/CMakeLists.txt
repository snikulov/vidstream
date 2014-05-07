cmake_minimum_required(VERSION 2.8)

project(vidstream C CXX)

find_package(Boost REQUIRED)
find_package(OpenCV REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

include_directories(include)
include_directories(include)

add_subdirectory(src)

# UT
enable_testing()
add_subdirectory(test)

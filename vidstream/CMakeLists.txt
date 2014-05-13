cmake_minimum_required(VERSION 2.8)

project(vidstream C CXX)

# set output dir for binary
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set( Boost_USE_STATIC_LIBS ON )
find_package(Boost COMPONENTS thread system program_options unit_test_framework REQUIRED)
find_package(OpenCV REQUIRED)

if(Boost_FOUND)
    add_definitions( -DBOOST_ALL_NO_LIB )
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

if(UNIX)
    message(STATUS "Build for Linux")
    add_definitions( -DBUILD_FOR_LINUX )
elseif(WIN32)
    message(STATUS "Build for Windows")
endif()


# externals project
add_subdirectory(ext/nanomsg)
include_directories(ext/nanomsg/src)

include_directories(include)
add_subdirectory(src)

# UT
enable_testing()
add_subdirectory(test)

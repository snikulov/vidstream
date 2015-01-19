cmake_minimum_required(VERSION 2.8)

project(vidstream C CXX)

# set output dir for binary
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(WITH_TEST "Build UnitTests" OFF)
option(USE_QT5 "Use qt5 for build" OFF)
option(WITH_ITPP "Build and use ITPP" ON)
option(WITH_CAPTURE_UI "Build with capture window" ON)
option(WITH_ASAN "Build with address sanitizer" OFF)

find_package(Threads REQUIRED)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W")

    if (WITH_ASAN)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    endif()
endif()

link_directories(${CMAKE_BINARY_DIR})

if(WIN32)
    set( Boost_USE_STATIC_LIBS ON )
endif()
if(MSVC)
    add_definitions(/EHsc)
endif()
add_definitions(-DBOOST_SYSTEM_NO_DEPRECATED )
find_package(Boost COMPONENTS random thread chrono system program_options unit_test_framework REQUIRED)

#find_package(OpenCV)

if(Boost_FOUND)
    #    add_definitions( -DBOOST_ALL_NO_LIB )
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()


# now enabled to cleanup old code
add_definitions( -DBUILD_FOR_LINUX )

if(UNIX)
    message(STATUS "Build for Linux")
elseif(WIN32)
    message(STATUS "Build for Windows")
endif()

file(GLOB_RECURSE GLB_HDR_FILES include/*.h*)
source_group("Global_HEADERS" FILES ${GLB_HDR_FILES})

#-------------------------------------------
# external projects
#-------------------------------------------
set(DEPS_LOCATION "${CMAKE_BINARY_DIR}/deps")

include_directories(${DEPS_LOCATION}/include)
link_directories(${DEPS_LOCATION}/lib)
link_directories(${DEPS_LOCATION}/lib64)

add_subdirectory(ext/libjpeg-turbo)

# ffmpeg from ext/ffmpeg
add_subdirectory(ext/ffmpeg)

# nanomsg build from ext/nanomsg
add_subdirectory(ext/nanomsg)

# log4cplus build from ext/log4cplus
add_subdirectory(ext/log4cplus)
set(LOG4CPLUS_LIBRARIES log4cplusS)

add_subdirectory(ext/opencv)

# itpp build from ext/itpp
if(WITH_ITPP)
    add_subdirectory(ext/itpp)
    add_definitions(-DBUILD_WITH_ITPP)
    set(ITPP_LIBRARIES itpp)
endif()

include_directories(include)
add_subdirectory(src)

# UT
if (WITH_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

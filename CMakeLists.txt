cmake_minimum_required(VERSION 2.8)

project(vidstream C CXX)

# set output dir for binary
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_property( GLOBAL PROPERTY USE_FOLDERS ON)

option(WITH_TEST "Build UnitTests" ON)
option(USE_QT5 "Use qt5 for build" OFF)
option(WITH_LOG4CPLUS "Build and use log4cplus" ON)
option(WITH_ITPP "Build and use ITPP" ON)
option(WITH_CAPTURE_UI "Build with capture window" ON)
option(WITH_ASAN "Build with address sanitizer" OFF)

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
find_package(OpenCV REQUIRED)

if(Boost_FOUND)
    #    add_definitions( -DBOOST_ALL_NO_LIB )
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()


# now enabled to cleanup old code
add_definitions( -DBUILD_FOR_LINUX )

if(UNIX)
    message(STATUS "Build for Linux")
    add_definitions(-Wall)
elseif(WIN32)
    message(STATUS "Build for Windows")
endif()

file(GLOB_RECURSE GLB_HDR_FILES include/*.h*)
source_group("Global_HEADERS" FILES ${GLB_HDR_FILES})

#-------------------------------------------
# external projects
#-------------------------------------------

#-------------------------------------------
# nanomsg build from ext/nanomsg
#-------------------------------------------
set(WITH_UT OFF)
add_subdirectory(ext/nanomsg)
include_directories(ext/nanomsg/src)

#-------------------------------------------
# log4cplus build from ext/log4cplus
#-------------------------------------------
if(WITH_LOG4CPLUS)
    set(BUILD_SHARED_LIBS OFF)
    set(UNICODE_DEFAULT OFF)
    set(UNICODE OFF)
    add_subdirectory(ext/log4cplus)
    include_directories(ext/log4cplus/include ${CMAKE_BINARY_DIR}/ext/log4cplus/include)
    set(LOG4CPLUS_LIBRARIES log4cplusS)
endif()
#-------------------------------------------
# itpp build from ext/itpp
#-------------------------------------------
if(WITH_ITPP)
    set(HTML_DOCS OFF)
    add_subdirectory(ext/itpp)
    include_directories(ext/itpp ${CMAKE_BINARY_DIR})
    set(ITPP_LIBRARIES "itpp")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(ITPP_LIBRARIES "${ITPP_LIBRARIES}_debug")
    endif()

endif()

if(WITH_CH_DBG)
    add_definitions(-DCHANNEL_DEBUG)
endif()

include_directories(include)
add_subdirectory(src)

# UT
if (WITH_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

cmake_minimum_required(VERSION 2.8)
set(TARGET_NAME ctrlsrv)
project(${TARGET_NAME} CXX)

include_directories(hdr)

set(SRC_FILES
    src/main.cpp
)

add_executable(${TARGET_NAME} ${SRC_FILES})

target_link_libraries(${TARGET_NAME}
    ${Boost_LIBRARIES}
    nanomsg
    )

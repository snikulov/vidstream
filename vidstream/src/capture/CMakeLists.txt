cmake_minimum_required(VERSION 2.8)
set(TARGET_NAME capture)
project(${TARGET_NAME} CXX)

include_directories(hdr)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SRC_FILES
    src/main_capture.cpp
)

add_executable(${TARGET_NAME} ${SRC_FILES} ${GLB_HDR_FILES})

if(WITH_CAPTURE_UI)
    set_target_properties(${TARGET_NAME} PROPERTIES COMPILE_DEFINITIONS "CAPTURE_UI")
endif()

target_link_libraries(${TARGET_NAME}
    ${OpenCV_LIBS} ${Boost_LIBRARIES}
    split
    corrupt
    nanomsg
    )
if(UNIX)
    target_link_libraries(${TARGET_NAME}
        bch
    )
endif()

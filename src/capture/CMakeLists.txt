cmake_minimum_required(VERSION 2.8)
set(TARGET_NAME capture)
project(${TARGET_NAME} CXX)

include_directories(hdr)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SRC_FILES
    src/main_capture.cpp
)

add_executable(${TARGET_NAME} ${SRC_FILES} ${GLB_HDR_FILES})

add_dependencies(${TARGET_NAME} ext.nanomsg ext.opencv ext.log4cplus)

lint_check_sources(${TARGET_NAME} ${SRC_FILES})

if(WITH_CAPTURE_UI)
    set_target_properties(${TARGET_NAME} PROPERTIES COMPILE_DEFINITIONS "CAPTURE_UI")
endif()

target_link_libraries(${TARGET_NAME}
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
    ${LOG4CPLUS_LIBRARIES}
    ${ITPP_LIBRARIES}
    split
    corrupt
    nanomsg
    channel
    utils
    bch
    )

target_link_libraries(${TARGET_NAME} opencv_core opencv_highgui opencv_imgcodecs opencv_imgproc opencv_video opencv_videoio)
target_link_libraries(${TARGET_NAME} ${CMAKE_THREAD_LIBS_INIT})

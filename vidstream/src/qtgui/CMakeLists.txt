cmake_minimum_required(VERSION 2.8)
set(TARGET_NAME qtgui)
project(${TARGET_NAME} CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


find_package(Qt5Widgets REQUIRED)

include_directories(${Qt5Widgets_INCLUDES})
add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

include_directories(hdr)

set(HDR_FILES
    hdr/mainwindow.h
    hdr/worker.hpp
    hdr/service_worker.hpp
    hdr/ctrlsrv.hpp
)

set(SRC_FILES
    src/main.cpp
    src/mainwindow.cpp
    src/service_worker.cpp
    src/cfg_ui_utils.cpp
)

set(UI_FILES
    ui/mainwindow.ui
)
qt5_wrap_ui(UI_SRC_FILES ${UI_FILES})

add_executable(${TARGET_NAME} ${SRC_FILES} ${UI_SRC_FILES} ${MOC_SRC} ${HDR_FILES} ${GLB_HDR_FILES})

target_link_libraries(${TARGET_NAME}
   ${Qt5Widgets_LIBRARIES}
   ${Boost_LIBRARIES}
   ${OpenCV_LIBS}
   nanomsg
   split
   )
if(UNIX)
    target_link_libraries(${TARGET_NAME}
        bch
    )
endif()

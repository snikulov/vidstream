cmake_minimum_required(VERSION 2.8)
set(TARGET_NAME qtgui)
project(${TARGET_NAME} CXX C)

#set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


#find_package(Qt5Widgets REQUIRED)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})


#include_directories(${Qt5Widgets_INCLUDES})
add_definitions(-DNOMINMAX)
#add_definitions(${Qt5Widgets_DEFINITIONS})
#set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

include_directories(hdr)

set(HDR_FILES
    hdr/mainwindow.h
    hdr/worker.hpp
    hdr/service_worker.hpp
    hdr/ctrlsrv.hpp
    hdr/jpeg_receiver.hpp
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

#qt5_wrap_ui(UI_SRC_FILES ${UI_FILES})

QT4_WRAP_UI(UI_SRC_FILES ${UI_FILES})
QT4_WRAP_CPP(MOC_SRC ${HDR_FILES})

add_executable(${TARGET_NAME} ${SRC_FILES} ${UI_SRC_FILES} ${MOC_SRC} ${HDR_FILES} ${GLB_HDR_FILES})

target_link_libraries(${TARGET_NAME}
#   ${Qt5Widgets_LIBRARIES}
   ${QT_LIBRARIES}
   ${OpenCV_LIBS}
   nanomsg
   split
   corrupt
#   boost_random
   ${Boost_LIBRARIES}
   )
if(UNIX)
    target_link_libraries(${TARGET_NAME}
        bch
    )
endif()

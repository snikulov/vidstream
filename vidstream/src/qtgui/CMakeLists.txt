cmake_minimum_required(VERSION 2.8)
set(TARGET_NAME qtgui)
project(${TARGET_NAME} CXX)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

# TODO: check on windows - something wrong with moc on CentOS
if(WIN32)
    set(CMAKE_AUTOMOC ON)
endif()

if(USE_QT5)
    cmake_policy(SET CMP0020 OLD)

    find_package(Qt5Widgets REQUIRED)
    include_directories(${Qt5Widgets_INCLUDES})
    add_definitions(${Qt5Widgets_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
else()
#    find_package(Qt4 REQUIRED)
    find_package(Qt4 COMPONENTS QtCore QtGui QtNetwork REQUIRED)
    include(${QT_USE_FILE})
endif()


add_definitions(-DNOMINMAX)

include_directories(hdr)
include_directories(${OpenCV_INCLUDE_DIRS})

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

if(USE_QT5)
    qt5_wrap_ui(UI_SRC_FILES ${UI_FILES})
else()
    QT4_WRAP_UI(UI_SRC_FILES ${UI_FILES})
    QT4_WRAP_CPP(MOC_SRC ${HDR_FILES})
endif()

add_executable(${TARGET_NAME} ${SRC_FILES} ${UI_SRC_FILES} ${MOC_SRC} ${HDR_FILES} ${GLB_HDR_FILES})

if(USE_QT5)
    target_link_libraries(${TARGET_NAME} ${Qt5Widgets_LIBRARIES})
else()
    target_link_libraries(${TARGET_NAME} ${QT_LIBRARIES})
endif()

target_link_libraries(${TARGET_NAME}
    ${Boost_LIBRARIES}
    ${OpenCV_LIBS}
    ${LOG4CPLUS_LIBRARIES}
    ${ITPP_LIBRARIES}
    nanomsg
    split
    corrupt
    channel
    utils
    bch
    )

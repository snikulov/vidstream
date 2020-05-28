cmake_minimum_required(VERSION 3.10)
set(TARGET_NAME qtgui)
project(${TARGET_NAME} CXX)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

# TODO: check on windows - something wrong with moc on CentOS
if(WIN32)
    set(CMAKE_AUTOMOC ON)
endif()

if(USE_QT5)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
#    set(CMAKE_AUTOUIC ON)

    find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

else()
    find_package(Qt4 COMPONENTS QtCore QtGui QtNetwork REQUIRED)
    include(${QT_USE_FILE})
endif()


add_definitions(-DNOMINMAX)

include_directories(hdr)
include_directories(${OpenCV_INCLUDE_DIRS})

set(HDR_FILES
    hdr/mainwindow.h
    hdr/settingsdialog.h
    hdr/worker.hpp
    hdr/service_worker.hpp
    hdr/ctrlsrv.hpp
    hdr/jpeg_receiver.hpp
    hdr/config_iface.hpp
)

set(SRC_FILES
    src/main.cpp
    src/mainwindow.cpp
    src/settingsdialog.cpp
    src/service_worker.cpp
    src/cfg_ui_utils.cpp
    src/config_iface.cpp
)

set(UI_FILES
    ui/mainwindow.ui
    ui/settingsdialog.ui
)

if(USE_QT5)
    qt5_wrap_ui(UI_SRC_FILES ${UI_FILES})
    add_executable(${TARGET_NAME} ${SRC_FILES} ${UI_SRC_FILES} ${HDR_FILES} ${GLB_HDR_FILES})
else()
    QT4_WRAP_UI(UI_SRC_FILES ${UI_FILES})
    QT4_WRAP_CPP(MOC_SRC ${HDR_FILES})
    add_executable(${TARGET_NAME} ${SRC_FILES} ${UI_SRC_FILES} ${MOC_SRC} ${HDR_FILES} ${GLB_HDR_FILES})
endif()

add_dependencies(${TARGET_NAME} ext.nanomsg ext.opencv ext.log4cplus)

lint_check_sources(${TARGET_NAME} ${SRC_FILES})

if(USE_QT5)
    target_link_libraries(${TARGET_NAME} Qt5::Widgets Qt5::Network)
else()
    target_link_libraries(${TARGET_NAME} ${QT_LIBRARIES})
endif()

target_link_libraries(${TARGET_NAME}
    ${OpenCV_LIBS}
    ${LOG4CPLUS_LIBRARIES}
    ${ITPP_LIBRARIES}
    nanomsg
    split
    corrupt
    channel
    utils
    bch
    ${Boost_LIBRARIES}
    )

target_link_libraries(${TARGET_NAME} opencv_core opencv_highgui opencv_imgcodecs opencv_imgproc opencv_video opencv_videoio)
target_link_libraries(${TARGET_NAME} ${CMAKE_THREAD_LIBS_INIT})

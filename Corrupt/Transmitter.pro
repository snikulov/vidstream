#-------------------------------------------------
#
# Project created by QtCreator 2014-04-13T19:28:04
#
#-------------------------------------------------

QT       += core

#QT       -= gui

TARGET = Transmitter
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    main_transmitter.cpp \
    settings.cpp \
    avhandler.cpp \
    thread_encode.cpp \
    thread_loader.cpp \
    thread_packetize.cpp \
    thread_send.cpp \
    bch.cpp \
    bitmap.cpp \
    corrupt.cpp \
    ecc.cpp \
    interlace.cpp \
    jpegops.cpp \
    restartblock.cpp \
    split.cpp \
    statcollector.cpp \
    transport.cpp

HEADERS += \
    settings.h \
    avhandler.h \
    thread_packetize.h \
    thread_loader.h \
    thread_send.h \
    thread_encode.h \
    bch.h \
    bitmap.h \
    corrupt.h \
    ecc.h \
    interlace.h \
    jpegops.h \
    membuf.h \
    restartblock.h \
    split.h \
    statcollector.h \
    threaded_coder.h \
    transport.h

LIBS += -ljpeg -lavcodec -lavformat -lavdevice -lavutil -lswscale -lrt \
        -lopencv_core -lopencv_imgproc -lopencv_video -lopencv_highgui

QMAKE_CFLAGS +=
QMAKE_CXXFLAGS += -DGENERATE_HEADER -g -O2 -std=c++11 -Wall -Wno-write-strings

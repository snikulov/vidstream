#-------------------------------------------------
#
# Project created by QtCreator 2014-02-02T20:26:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = corrupt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    bch.cpp \
    corrupt.cpp \
    ecc.cpp \
    restartblock.cpp \
    split.cpp \
    jpegops.cpp \
    statcollector.cpp \
    bitmap.cpp \
    avhandler.cpp \
    interlace.cpp \
    transport.cpp \
    thread_encode.cpp \
    thread_decode.cpp \
    thread_send.cpp \
    thread_read.cpp \
    thread_packetize.cpp \
    thread_reassemble.cpp \
    settings.cpp \
    thread_loader.cpp

HEADERS  += mainwindow.h \
    bch.h \
    corrupt.h \
    ecc.h \
    membuf.h \
    restartblock.h \
    split.h \
    jpegops.h \
    statcollector.h \
    bitmap.h \
    avhandler.h \
    interlace.h \
    transport.h \
    threaded_coder.h \
    thread_packetize.h \
    thread_reassemble.h \
    thread_encode.h \
    thread_decode.h \
    thread_send.h \
    thread_read.h \
    queue_params.h \
    settings.h \
    thread_loader.h

LIBS += -ljpeg -lavcodec -lavformat -lavdevice -lavutil -lswscale -lrt

FORMS    += mainwindow.ui \
    settingsdialog.ui

#SUBDIRS += \
#    test.pro

OTHER_FILES += \
    Corrupt.pro.user

QMAKE_CFLAGS +=
QMAKE_CXXFLAGS += -DGENERATE_HEADER -g -O2 -std=c++11 -Wall -Wno-write-strings

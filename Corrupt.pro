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
    receiverthread.cpp \
    restartblock.cpp \
    senderthread.cpp \
    split.cpp \
    transceiver.cpp \
    jpegops.cpp \
    statcollector.cpp \
    settingsdialog.cpp \
    bitmap.cpp \
    avhandler.cpp \
    interlace.cpp

HEADERS  += mainwindow.h \
    bch.h \
    corrupt.h \
    ecc.h \
    membuf.h \
    receiverthread.h \
    restartblock.h \
    senderthread.h \
    split.h \
    transceiver.h \
    jpegops.h \
    statcollector.h \
    settingsdialog.h \
    bitmap.h \
    avhandler.h \
    interlace.h

LIBS += -ljpeg -lavcodec -lavformat -lavdevice -lavutil -lswscale

FORMS    += mainwindow.ui \
    settingsdialog.ui

#SUBDIRS += \
#    test.pro

OTHER_FILES += \
    Corrupt.pro.user

QMAKE_CFLAGS += -O3
QMAKE_CXXFLAGS += -DGENERATE_HEADER -g -O2 -std=c++11 -Wno-write-strings

#-------------------------------------------------
#
# Project created by QtCreator 2016-04-10T10:32:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = detector
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    detector.cpp

HEADERS  += mainwindow.h \
    detector.h

FORMS    += mainwindow.ui


DISTFILES += \
    ../../../../Users/rising-edge/Pictures/bluesky.jpg

RESOURCES +=

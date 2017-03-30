QT += core
QT -= gui

CONFIG += c++11

TARGET = TimeSynchronizer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    timesynchronizer.cpp

HEADERS += \
    timesynchronizer.h

DISTFILES += \
    TimeSynchronizer.pri

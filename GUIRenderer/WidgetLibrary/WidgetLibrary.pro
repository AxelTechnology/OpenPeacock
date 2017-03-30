QT       -=  gui
QT += widgets

#TARGET = WidgetLibrary
TEMPLATE = lib

DEFINES += WIDGETLIBRARY_LIBRARY

HEADERS += \
    abstractwidget.h \
    widgetconfiguration.h \
    widgetposition.h \
    widgetdata.h

SOURCES += \
    abstractwidget.cpp \
    widgetconfiguration.cpp \
    widgetposition.cpp \
    widgetdata.cpp

DISTFILES += \
    WidgetLibrary.pri



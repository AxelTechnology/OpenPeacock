#QT       -=  gui
#QT += widgets

#TARGET = WidgetLibrary
#TEMPLATE = lib

DEFINES += WIDGETLIBRARY_LIBRARY


INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/abstractwidget.h \
    $$PWD/widgetconfiguration.h \
    $$PWD/widgetposition.h \
    $$PWD/widgetdata.h \
    $$PWD/widgetfactory.h \
    $$PWD/singleton.h \
    $$PWD/call_once.h \
    $$PWD/logger.h \
    $$PWD/axelpainter.h

SOURCES += \
    $$PWD/abstractwidget.cpp \
    $$PWD/widgetconfiguration.cpp \
    $$PWD/widgetposition.cpp \
    $$PWD/widgetdata.cpp \
    $$PWD/widgetfactory.cpp \
    $$PWD/logger.cpp \
    $$PWD/axelpainter.cpp

RESOURCES += \
    $$PWD/widgetsresources.qrc


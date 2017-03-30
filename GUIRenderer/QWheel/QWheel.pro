QT += widgets

TEMPLATE = lib

HEADERS += \
    wheelconfiguration.h \
    wheeldata.h \
    qwheel.h

SOURCES += \
    wheelconfiguration.cpp \
    wheeldata.cpp \
    qwheel.cpp


include(../WidgetLibrary/WidgetLibrary.pri)

DISTFILES += \
    QWheel.pri

RESOURCES += \
    qwheelresources.qrc

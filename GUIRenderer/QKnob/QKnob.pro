TEMPLATE = lib


HEADERS += \
    knobconfiguration.h \
    knobdata.h \
    qknob.h

SOURCES += \
    knobconfiguration.cpp \
    knobdata.cpp \
    qknob.cpp \
    qknobdrawfunctions.cpp


include(../WidgetLibrary/WidgetLibrary.pri)

DISTFILES += \
    QKnob.pri


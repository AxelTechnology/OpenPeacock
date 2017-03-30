QT += widgets

TEMPLATE = lib

HEADERS += \
    mixerlabel.h \
    mixerlabelconfiguration.h

SOURCES += \
    mixerlabel.cpp \
    mixerlabelconfiguration.cpp


include(../WidgetLibrary/WidgetLibrary.pri)

DISTFILES += \
    MixerLabel.pri

RESOURCES += \
    mixerlabelresources.qrc

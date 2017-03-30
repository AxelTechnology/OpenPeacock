TEMPLATE = lib
QT += printsupport
CONFIG += console

HEADERS += \
    jsonserializer.h \
    guidata.h

SOURCES += \
    jsonserializer.cpp \
    guidata.cpp


include(../GUIRenderer/WidgetLibrary/WidgetLibrary.pri)
include(../GUIRenderer/OnairclockWidget/OnairclockWidget.pri)
include(../GUIRenderer/AxelLabel/AxelLabel.pri)
include(../GUIRenderer/MultiImage/MultiImage.pri)
include(../GUIRenderer/QGraph/QGraph.pri)
include(../GUIRenderer/QKnob/QKnob.pri)
include(../GUIRenderer/QWheel/QWheel.pri)
include(../GUIRenderer/AxelPanel/AxelPanel.pri)
include(../GUIRenderer/VuMeter/VuMeter.pri)

DISTFILES += \
    GUISerializer.pri

QT += widgets printsupport network
QT += quick qml quickwidgets

@
OnDesktop:DEFINES += ON_DESKTOP
OnMixer:DEFINES += ON_MIXER
@

TEMPLATE = app
TARGET = AxelGUI

CONFIG += console

RESOURCES += \
    resources.qrc

HEADERS += \
    udpsocket.h \
    axelframe.h \
    guilayoutmanager.h \
    keyboardreader.h \
    ilayoutmanager.h

SOURCES += \
    main.cpp \
    udpsocket.cpp \
    axelframe.cpp \
    guilayoutmanager.cpp \
    keyboardreader.cpp

QMAKE_PROJECT_NAME = axelgui

target.path = $$[QT_INSTALL_EXAMPLES]/axelgui
INSTALLS += target

include(../GUIWidgets.pri)

include(../../GUISerializer/GUISerializer.pri)
include(../../SystemManager/SystemManager.pri)

DISTFILES += \
    Axel_pac-man.png


greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets designer

greaterThan(QT_MINOR_VERSION, 4) {
DEFINES += BiggerThan54
}

} else {
    CONFIG += designer
}

QT += printsupport

CONFIG      += plugin debug_and_release
#CONFIG      += plugin release

TEMPLATE    = lib
TARGET      = $$qtLibraryTarget($$TARGET)
target.path = $$[createWidget]/designer
DESTDIR +=    $$[QT_INSTALL_PLUGINS]/designer
#DESTDIR +=    /home/prisco/QtSuite/Tools/QtCreator/lib/Qt/plugins/designer
INSTALLS    += target

HEADERS     =   \                
                qgraphplugin.h \
                qknobplugin.h \
                qwheelplugin.h \
                onairclockwidgetplugin.h \
                axelwidgets.h \
                axellabelplugin.h \
                multiimageplugin.h \
                vumeterplugin.h \
                axelpanelplugin.h \
                metadataplugin.h


SOURCES     =   \
                qgraphplugin.cpp \
                qknobplugin.cpp \
                qwheelplugin.cpp \
                onairclockwidgetplugin.cpp \
                axellabelplugin.cpp \
                axelwidgets.cpp \
                multiimageplugin.cpp \
                vumeterplugin.cpp \
                axelpanelplugin.cpp \
                metadataplugin.cpp


RESOURCES   = icons.qrc
LIBS        += -L. 


include(../../GUIRenderer/GUIWidgets.pri)
include(../../GUISerializer/GUISerializer.pri)

DISTFILES += \
    AxelGUIPlugins.pri

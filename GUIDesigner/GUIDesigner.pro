#-------------------------------------------------
#
# Project created by QtCreator 2016-04-11T11:44:34
#
#-------------------------------------------------

QT       += core gui printsupport network designer
#QT += quick qml quickwidgets



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets uitools

TARGET = GUIDesigner
TEMPLATE = app
CONFIG += console debug_and_release

SOURCES += main.cpp \
        mainwindow.cpp \
    axeluiloader.cpp

HEADERS  += mainwindow.h \
    axeluiloader.h

FORMS    += mainwindow.ui
#    testwindow2.ui \
#    testMultipleGraphs.ui \
#    testwindow1.ui \
#    testwindow4.ui \
#    test100VuMeters.ui \
#    testEveryWidget.ui \
#    testGraph.ui \
#    HOME-4.ui \
#    EQ2.ui \
#    EQ-ADVSET1.ui \
#    EQ-ADVSET2.ui \
#    EQ-ADVSET3.ui \
#    EQ1.ui


include(../GUIRenderer/GUIWidgets.pri)

include(../GUISerializer/GUISerializer.pri)


DISTFILES +=

RESOURCES += \
    guidesignerresources.qrc



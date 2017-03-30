TEMPLATE = lib


HEADERS += \
    graphconfiguration.h \
    graphdata.h \
    qcustomplot.h \
    qgraph.h

SOURCES += \
    graphconfiguration.cpp \
    graphdata.cpp \
    qcustomplot.cpp \
    qgraph.cpp

DISTFILES += \
    QGraph.pri


include(../WidgetLibrary/WidgetLibrary.pri)

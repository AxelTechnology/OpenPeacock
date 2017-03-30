#include "widgetfactory.h"

#include "onairclockwidget.h"
#include "graphdata.h"
#include "axellabel.h"
#include "qgraph.h"
#include "qknob.h"
#include "qwheel.h"
#include "multiimage.h"
#include "vumeter.h"
#include "axelpanel.h"
#include "logger.h"
#include "metadata.h"

WidgetFactory::WidgetFactory()
{

}

void WidgetFactory::Initialize()
{
    //QFontDatabase::addApplicationFont(":/CALIBRIB.TTF");
    QFontDatabase::addApplicationFont(":/DS-DIGIB.TTF");
}

QWidget * WidgetFactory::CreateWidgetByClassName(QString className, QWidget *parent)
{
    QWidget* widget = 0;

    if(className == "Frame")
        widget = new QFrame(parent);
    else if(className == "AxelLabel")
            widget = new AxelLabel(parent);
    else if(className == "MultiImage")
            widget = new MultiImage(parent);
    else if(className == "OnairclockWidget")
            widget = new OnairclockWidget(parent);
    else if(className == "QWheel")
            widget = new QWheel(parent);
    else if(className == "QGraph")
            widget = new QGraph(parent);
    else if(className == "QKnob")
            widget = new QKnob(parent);
    else if(className == "VuMeter")
            widget = new VuMeter(parent);
    else if(className == "AxelPanel")
            widget = new AxelPanel(parent);
    else if(className == "Metadata")
            widget = new Metadata(parent);
    else
        Logger::Instance()->AddLog("Unrecognized widget class name: " + className, LoggerLevel_ERROR, "WIDGET_CLASS");



    return widget;
}

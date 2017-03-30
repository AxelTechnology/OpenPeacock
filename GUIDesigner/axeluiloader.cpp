#include "axeluiloader.h"
#include "widgetfactory.h"
#include "axelpanel.h"
#include "logger.h"

#include <QtDebug>
#include <QWidget>

AxelUiLoader::AxelUiLoader(QObject * parent = 0) : QUiLoader(parent)
{

}


QWidget* AxelUiLoader::createWidget(const QString &className, QWidget *parent, const QString &name)
{    
    QWidget* widget;

    Logger::Instance()->AddLog("Creating widget " + name + " ...", LoggerLevel_DEBUG);

    widget = WidgetFactory::CreateWidgetByClassName(className, parent);

    if(widget != 0)
        widget->setObjectName(name);
    else
        widget = QUiLoader::createWidget(className, parent, name);

    Logger::Instance()->AddLog("Created " + name, LoggerLevel_DEBUG);

    // do stuff with className, name, widget, etc
    return widget;
}


#include "onairclockwidget.h"
#include "onairclockwidgetplugin.h"

#include <QtPlugin>

OnairclockWidgetPlugin::OnairclockWidgetPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void OnairclockWidgetPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool OnairclockWidgetPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *OnairclockWidgetPlugin::createWidget(QWidget *parent)
{
    return new OnairclockWidget(parent);
}

QString OnairclockWidgetPlugin::name() const
{
    return QLatin1String("OnairclockWidget");
}

QString OnairclockWidgetPlugin::group() const
{
    return QLatin1String("");
}

QIcon OnairclockWidgetPlugin::icon() const
{
    return QIcon(QLatin1String(":/OnairclockWidget.png"));
}

QString OnairclockWidgetPlugin::toolTip() const
{
    return QLatin1String("");
}

QString OnairclockWidgetPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool OnairclockWidgetPlugin::isContainer() const
{
    return false;
}

QString OnairclockWidgetPlugin::domXml() const
{
    return QLatin1String("<widget class=\"OnairclockWidget\" name=\"onairclockWidget\">\n</widget>\n");
}

QString OnairclockWidgetPlugin::includeFile() const
{
    return QLatin1String("onairclockwidget.h");
}


#include "qwheel.h"
#include "qwheelplugin.h"

#include <QtPlugin>

QWheelPlugin::QWheelPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void QWheelPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool QWheelPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *QWheelPlugin::createWidget(QWidget *parent)
{
    return new QWheel(parent);
}

QString QWheelPlugin::name() const
{
    return QLatin1String("QWheel");
}

QString QWheelPlugin::group() const
{
    return QLatin1String("");
}

QIcon QWheelPlugin::icon() const
{
    return QIcon(QLatin1String(":/QWheel.png"));
}

QString QWheelPlugin::toolTip() const
{
    return QLatin1String("");
}

QString QWheelPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool QWheelPlugin::isContainer() const
{
    return false;
}

QString QWheelPlugin::domXml() const
{
    return QLatin1String("<widget class=\"QWheel\" name=\"qWheel\">\n</widget>\n");
}

QString QWheelPlugin::includeFile() const
{
    return QLatin1String("qwheel.h");
}


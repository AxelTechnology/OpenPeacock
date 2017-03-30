#include "multiimageplugin.h"
#include "multiimage.h"

#include <QtPlugin>

MultiImagePlugin::MultiImagePlugin(QObject *parent)
    : QObject(parent)
{

}


void MultiImagePlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool MultiImagePlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *MultiImagePlugin::createWidget(QWidget *parent)
{
    return new MultiImage(parent);
}

QString MultiImagePlugin::name() const
{
    return QLatin1String("MultiImage");
}

QString MultiImagePlugin::group() const
{
    return QLatin1String("");
}

QIcon MultiImagePlugin::icon() const
{
    return QIcon(QLatin1String(":/MultiImage.png"));
}

QString MultiImagePlugin::toolTip() const
{
    return QLatin1String("");
}

QString MultiImagePlugin::whatsThis() const
{
    return QLatin1String("");
}

bool MultiImagePlugin::isContainer() const
{
    return false;
}

QString MultiImagePlugin::domXml() const
{
    return QLatin1String("<widget class=\"MultiImage\" name=\"multiImage\">\n</widget>\n");
}

QString MultiImagePlugin::includeFile() const
{
    return QLatin1String("multiimage.h");
}


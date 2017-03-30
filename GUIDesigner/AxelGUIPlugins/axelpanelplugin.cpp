#include "axelpanelplugin.h"
#include "axelpanel.h"

AxelPanelPlugin::AxelPanelPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void AxelPanelPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool AxelPanelPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *AxelPanelPlugin::createWidget(QWidget *parent)
{
    return new AxelPanel(parent);
}

QString AxelPanelPlugin::name() const
{
    return QLatin1String("AxelPanel");
}

QString AxelPanelPlugin::group() const
{
    return QLatin1String("");
}

QIcon AxelPanelPlugin::icon() const
{
    return QIcon(QLatin1String(":/AxelPanel.png"));
}

QString AxelPanelPlugin::toolTip() const
{
    return QLatin1String("");
}

QString AxelPanelPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool AxelPanelPlugin::isContainer() const
{
    return true;
}

QString AxelPanelPlugin::domXml() const
{
    return QLatin1String("<widget class=\"AxelPanel\" name=\"axelpanel\">\n</widget>\n");
}


QString AxelPanelPlugin::includeFile() const
{
    return QLatin1String("axelpanel.h");
}


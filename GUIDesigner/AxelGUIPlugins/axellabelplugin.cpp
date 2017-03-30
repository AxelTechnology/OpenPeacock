#include "axellabel.h"
#include "axellabelplugin.h"

#include <QtPlugin>

AxelLabelPlugin::AxelLabelPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void AxelLabelPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool AxelLabelPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *AxelLabelPlugin::createWidget(QWidget *parent)
{
    return new AxelLabel(parent);
}

QString AxelLabelPlugin::name() const
{
    return QLatin1String("AxelLabel");
}

QString AxelLabelPlugin::group() const
{
    return QLatin1String("");
}

QIcon AxelLabelPlugin::icon() const
{
    return QIcon(QLatin1String(":/AxelLabel.png"));
}

QString AxelLabelPlugin::toolTip() const
{
    return QLatin1String("");
}

QString AxelLabelPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool AxelLabelPlugin::isContainer() const
{
    return false;
}

QString AxelLabelPlugin::domXml() const
{
    return QLatin1String("<widget class=\"AxelLabel\" name=\"axelLabel\">\n</widget>\n");
}


QString AxelLabelPlugin::includeFile() const
{
    return QLatin1String("axellabel.h");
}


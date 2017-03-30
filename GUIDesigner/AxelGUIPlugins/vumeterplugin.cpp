#include "vumeterplugin.h"
#include "vumeter.h"

VuMeterPlugin::VuMeterPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}


void VuMeterPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool VuMeterPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *VuMeterPlugin::createWidget(QWidget *parent)
{
    return new VuMeter(parent);
}

QString VuMeterPlugin::name() const
{
    return QLatin1String("VuMeter");
}

QString VuMeterPlugin::group() const
{
    return QLatin1String("");
}

QIcon VuMeterPlugin::icon() const
{
    return QIcon(QLatin1String(":/VuMeter.png"));
}

QString VuMeterPlugin::toolTip() const
{
    return QLatin1String("");
}

QString VuMeterPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool VuMeterPlugin::isContainer() const
{
    return false;
}

QString VuMeterPlugin::domXml() const
{
    return QLatin1String("<widget class=\"VuMeter\" name=\"vuMeter\">\n</widget>\n");
}

QString VuMeterPlugin::includeFile() const
{
    return QLatin1String("vumeter.h");
}


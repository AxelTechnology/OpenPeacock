#include "metadataplugin.h"
#include "metadata.h"

#include <QtPlugin>

MetadataPlugin::MetadataPlugin(QObject *parent)
    : QObject(parent)
{

}


void MetadataPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool MetadataPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *MetadataPlugin::createWidget(QWidget *parent)
{
    return new Metadata(parent);
}

QString MetadataPlugin::name() const
{
    return QLatin1String("Metadata");
}

QString MetadataPlugin::group() const
{
    return QLatin1String("");
}

QIcon MetadataPlugin::icon() const
{
    return QIcon(QLatin1String(":/Metadata.png"));
}

QString MetadataPlugin::toolTip() const
{
    return QLatin1String("");
}

QString MetadataPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool MetadataPlugin::isContainer() const
{
    return false;
}

QString MetadataPlugin::domXml() const
{
    return QLatin1String("<widget class=\"Metadata\" name=\"metadata\">\n</widget>\n");
}

QString MetadataPlugin::includeFile() const
{
    return QLatin1String("metadata.h");
}


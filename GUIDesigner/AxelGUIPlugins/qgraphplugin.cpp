#include "qgraph.h"
#include "qgraphplugin.h"

#include <QtPlugin>

QGraphPlugin::QGraphPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void QGraphPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool QGraphPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *QGraphPlugin::createWidget(QWidget *parent)
{
    return new QGraph(parent);
}

QString QGraphPlugin::name() const
{
    return QLatin1String("QGraph");
}

QString QGraphPlugin::group() const
{
    return QLatin1String("");
}

QIcon QGraphPlugin::icon() const
{
    return QIcon(QLatin1String(":/QGraph.png"));
}

QString QGraphPlugin::toolTip() const
{
    return QLatin1String("");
}

QString QGraphPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool QGraphPlugin::isContainer() const
{
    return false;
}

QString QGraphPlugin::domXml() const
{
    return QLatin1String("<widget class=\"QGraph\" name=\"qGraph\">\n</widget>\n");
}

QString QGraphPlugin::includeFile() const
{
    return QLatin1String("qgraph.h");
}


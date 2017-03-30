#include "qknob.h"
#include "qknobplugin.h"

#include <QtPlugin>

QKnobPlugin::QKnobPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void QKnobPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool QKnobPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *QKnobPlugin::createWidget(QWidget *parent)
{
    return new QKnob(parent);
}

QString QKnobPlugin::name() const
{
    return QLatin1String("QKnob");
}

QString QKnobPlugin::group() const
{
    return QLatin1String("");
}

QIcon QKnobPlugin::icon() const
{
    return QIcon(QLatin1String(":/QKnob.png"));
}

QString QKnobPlugin::toolTip() const
{
    return QLatin1String("");
}

QString QKnobPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool QKnobPlugin::isContainer() const
{
    return false;
}

QString QKnobPlugin::domXml() const
{
    return QLatin1String("<widget class=\"QKnob\" name=\"qknob\">\n</widget>\n");
}

QString QKnobPlugin::includeFile() const
{
    return QLatin1String("qknob.h");
}


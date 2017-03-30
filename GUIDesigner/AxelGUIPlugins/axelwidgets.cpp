#include "axellabelplugin.h"
#include "onairclockwidgetplugin.h"
#include "qgraphplugin.h"
#include "qknobplugin.h"
#include "qwheelplugin.h"
#include "multiimageplugin.h"
#include "axelwidgets.h"
#include "vumeterplugin.h"
#include "axelpanelplugin.h"
#include "widgetfactory.h"
#include "metadataplugin.h"


AxelWidgets::AxelWidgets(QObject *parent)
    : QObject(parent)
{
    WidgetFactory::Initialize();
    m_widgets.append(new OnairclockWidgetPlugin(this));
    m_widgets.append(new AxelLabelPlugin(this));
    m_widgets.append(new QGraphPlugin(this));
    m_widgets.append(new QKnobPlugin(this));
    m_widgets.append(new QWheelPlugin(this));
    m_widgets.append(new MultiImagePlugin(this));
    m_widgets.append(new VuMeterPlugin(this));
    m_widgets.append(new AxelPanelPlugin (this));
    m_widgets.append(new MetadataPlugin (this));
}

QList<QDesignerCustomWidgetInterface*> AxelWidgets::customWidgets() const
{
    return m_widgets;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(mixerwidgetsplugin, MixerWidgets)
#endif // QT_VERSION < 0x050000

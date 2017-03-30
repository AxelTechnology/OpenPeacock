#ifndef VUMETERPLUGIN_H
#define VUMETERPLUGIN_H

#include <QObject>
#include <QWidget>

#ifndef BiggerThan54
#include <QDesignerCustomWidgetInterface>
#else
#include <QtUiPlugin/QDesignerCustomWidgetInterface>
#endif

class VuMeterPlugin : public QObject, public QDesignerCustomWidgetInterface
{

    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    VuMeterPlugin(QObject *parent = 0);

    bool isContainer() const;
    bool isInitialized() const;
    QIcon icon() const;
    QString domXml() const;
    QString group() const;
    QString includeFile() const;
    QString name() const;
    QString toolTip() const;
    QString whatsThis() const;
    QWidget *createWidget(QWidget *parent);
    void initialize(QDesignerFormEditorInterface *core);

private:
    bool m_initialized;
};

#endif // VUMETERPLUGIN_H

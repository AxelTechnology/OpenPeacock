#ifndef MIXERJSONSERIALIZER_H
#define MIXERJSONSERIALIZER_H

#include "guidata.h"

#include <QObject>
#include <axelpanel.h>

class JsonSerializer
{
public:
    JsonSerializer(QWidget *parent = 0);
    QList<QWidget*> * getSerializableWidgets(QWidget * currentLayout);
    QString SerializeData(GUIData* jsonData);
    QString SerializeVariablesData(GUIData* data);
    //GUIData* DeserializeData(QString jsonStringData);
    GUIData* DeserializeData(QJsonObject jsonObj);

    static QJsonObject GetVarsProperty(QString controllerData, QString controllerType, QString propertyName);

    static const int versionMajor = 1;
    static const int versionMinor = 2;

private:
    QWidget* _parent;
    QWidget *CreateWidget(QString widgetType, QJsonObject* widgetData);
    QWidget* GetObjectByName(QList<QWidget*>* widgetList, QString name);
    AxelPanel* ConvertToAxelPanel(QWidget* container);
    QString DoubleValueWorkaround(QString jsonData);

};

#endif // MIXERJSONSERIALIZER_H

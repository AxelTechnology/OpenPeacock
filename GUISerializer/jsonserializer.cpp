#include "jsonserializer.h"

#include "widgetfactory.h"
#include "abstractwidget.h"
#include "logger.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>


JsonSerializer::JsonSerializer(QWidget *parent)
{
    _parent = parent;
}



QList<QWidget *> *JsonSerializer::getSerializableWidgets(QWidget * currentLayout)
{
    QList<QWidget*> * widgetList = new QList<QWidget*>();
    QList<QWidget *> currentChildren = currentLayout->findChildren<QWidget *>();

    foreach(QWidget * widget, currentChildren )
    {

        QObject * widgetParent = widget->parent();

        if (widget->inherits("QGroupBox") || widget->inherits("QFrame"))
            widget = ConvertToAxelPanel(widget);

        if((widgetParent->inherits("AxelPanel") || widgetParent->inherits("QGroupBox") || widgetParent->inherits("QFrame")) && widget->inherits("AbstractWidget"))
        {
            QString parentName = ((QWidget*) widgetParent)->objectName();
            Logger::Instance()->AddLog("widget " + widget->objectName() + " is contained in " + parentName, LoggerLevel_DEBUG);

            ((AbstractWidget*)widget)->setParentObjectName(parentName);
        }


        //if(!widget->inherits("AbstractWidget"))
        //    continue;
        widgetList->append(widget);
    }

    return widgetList;
}




QString JsonSerializer::SerializeData(GUIData* jsonData)
{
    if (!jsonData)
        return QString();

    QJsonObject mainObj = QJsonObject();

    mainObj.insert("key", QJsonValue("SCREEN_LAYOUT"));
    mainObj.insert("layoutCreationTime", QJsonValue(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss")));
    mainObj.insert("versionMajor", QJsonValue(QString::number(versionMajor)));
    mainObj.insert("versionMinor", QJsonValue(QString::number(versionMinor)));

    mainObj.insert("totalWidth", QJsonValue(jsonData->totalSize().width()));
    mainObj.insert("totalHeight", QJsonValue(jsonData->totalSize().height()));


    QJsonArray widgets = QJsonArray();

    //QList<QWidget*> widgetList = *jsonData->widgetList();


    for(int i = 0; i< jsonData->widgetList()->count(); i++)
    {
        if(!jsonData->widgetList()->at(i)->inherits("AbstractWidget"))
            continue;
        AbstractWidget* w = (AbstractWidget*) jsonData->widgetList()->at(i);
        widgets.append(QJsonValue(*w->GetJsonData()));
     }
    mainObj.insert("widgets", QJsonValue(widgets));

    QJsonDocument* doc = new QJsonDocument(mainObj);
    return QString(doc->toJson());
}


QString JsonSerializer::SerializeVariablesData(GUIData* jsonData)
{
    if (!jsonData)
        return QString();

    QJsonObject mainObj = QJsonObject();

    QJsonArray variables = QJsonArray();

    //QList<QWidget*> widgetList = *jsonData->widgetList();

    for(int i = 0; i< jsonData->widgetList()->count(); i++)
    {
        if(!jsonData->widgetList()->at(i)->inherits("AbstractWidget"))
            continue;
        AbstractWidget* w = (AbstractWidget*) jsonData->widgetList()->at(i);
        QJsonObject* vars = w->GetJsonDataVars();
        if(vars != 0)
            variables.append(QJsonValue(*vars));
    }

    mainObj.insert("variables", QJsonValue(variables));

    QJsonDocument* doc = new QJsonDocument(mainObj);
    return DoubleValueWorkaround(QString(doc->toJson()));
}

QString JsonSerializer::DoubleValueWorkaround(QString jsonData)
{
    QString retVal;
    QStringList lines = jsonData.split('\n');

    foreach (const QString &line, lines)
    {
        if(line.contains("\"scale\":") && (!line.contains(".")))
        {
            QString fixedLine = line + ".0\n";
            retVal.append(fixedLine);
        }
        else
            retVal.append(line + "\n");
    }

    return retVal;
}


GUIData* JsonSerializer::DeserializeData(QJsonObject obj)
{
    Logger::Instance()->AddLog("Created QJsonDocument", LoggerLevel_DEBUG);

    //Getting main layout data
    int totalWidth = obj["totalWidth"].toInt();
    int totalHeight = obj["totalHeight"].toInt();

    GUIData* data = new GUIData();
    data->setTotalSize(QSize(totalWidth, totalHeight));

    if(!obj["widgets"].isArray())
        return data;

    Logger::Instance()->AddLog("Start creating widgets", LoggerLevel_DEBUG);

    //Getting widgets
    QJsonArray widgets = obj["widgets"].toArray();

    foreach (QJsonValue w, widgets) {
        if (!w.isObject())
            continue;
        QJsonObject wObj = w.toObject();

        Logger::Instance()->AddLog("Getting object type", LoggerLevel_DEBUG);
        QString type = wObj["type"].toString();

        Logger::Instance()->AddLog("Creating " + type, LoggerLevel_DEBUG);
        QWidget* widget = CreateWidget(type, &wObj);

        if(widget == 0)
            continue;

        if( widget->inherits("Metadata"))
            continue;

        //DEBUG
        if (widget->inherits("AxelLabel"))
            Logger::Instance()->AddLog("Adding AxelLabel " + widget->objectName(), LoggerLevel_DEBUG);

        Logger::Instance()->AddLog("Adding widget to list", LoggerLevel_DEBUG);
        data->widgetList()->append(widget);

    }

    Logger::Instance()->AddLog("Setting widgets parent", LoggerLevel_DEBUG);

    //Setting widgets parents
    for (int i = 0; i < data->widgetList()->count(); i++)
    {
        QWidget * w = data->widgetList()->at(i);
        if(w == 0)
            continue;
        if(!w->inherits("AbstractWidget"))
            continue;
        AbstractWidget* aWidget = (AbstractWidget*) w;
        if(aWidget->parentObjectName().isEmpty())
            continue;

        QWidget* parent = GetObjectByName(data->widgetList(), aWidget->parentObjectName());
        if(parent == 0)
        {
            aWidget->setParentObjectName(QString());
            continue;
        }
        aWidget->setParent(parent);
    }

    Logger::Instance()->AddLog("End parsing layout", LoggerLevel_DEBUG);

    return data;
}



//Get widget from list by its objectName
QWidget* JsonSerializer::GetObjectByName(QList<QWidget*>* widgetList, QString name)
{
    if (widgetList == 0)
        return 0;
    for (int i = 0; i < widgetList->count(); i++)
    {
        QWidget * w = widgetList->at(i);
        if(w->objectName() != name)
            continue;
        return w;
    }

    return 0;
}



QWidget* JsonSerializer::CreateWidget(QString widgetType, QJsonObject* widgetData)
{
    if(widgetType == 0)
        return 0;

    QWidget* widget;

    Logger::Instance()->AddLog("Creating " + widgetType + " instance", LoggerLevel_DEBUG);
    widget = WidgetFactory::CreateWidgetByClassName(widgetType, _parent);

    if(widget == 0)
    {
        Logger::Instance()->AddLog("Unknown widget, creating QWidget", LoggerLevel_ERROR, "WIDGET_TYPE");
        widget = new QWidget(_parent);
    }


    Logger::Instance()->AddLog("Setting widget geometry", LoggerLevel_DEBUG);
    if (widgetData != 0)
        widget->setGeometry((*widgetData)["x"].toInt(), (*widgetData)["y"].toInt(), (*widgetData)["width"].toInt(), (*widgetData)["height"].toInt());


    if( widget->inherits("AbstractWidget"))
    {
        Logger::Instance()->AddLog("Setting AbstractWidget data", LoggerLevel_DEBUG);
        if(widgetData != 0)
            ((AbstractWidget*)widget)->SetJsonData(widgetData);
    }    

    Logger::Instance()->AddLog("Created widget " + widget->objectName(), LoggerLevel_DEBUG);
    return widget;

}


AxelPanel* JsonSerializer::ConvertToAxelPanel(QWidget* container)
{
    if (container == 0)
        return 0;

    AxelPanel* retObj = new AxelPanel();
    retObj->setObjectName(container->objectName());
    retObj->setGeometry(container->geometry());
    retObj->setColorBg(QColor(0,0,0,0));

    return retObj;
}


QJsonObject JsonSerializer::GetVarsProperty(QString controllerData, QString controllerType, QString propertyName)
{
    QJsonObject retObj = QJsonObject();
    QJsonObject controllerObj = QJsonObject();

    if(!controllerData.isNull() && !controllerData.isEmpty())
    {
        QJsonDocument doc = QJsonDocument::fromJson(controllerData.toUtf8());
        if ((doc.isObject()) && (!doc.isEmpty()) && (!doc.isNull()))
        {
            controllerObj = doc.object();
//            if (controllerObj.contains("scale"))
//            {
//                QJsonValue scale = controllerObj["scale"];
//                double scaleVal = scale.toDouble();
//                scale = QJsonValue(scaleVal);
//                controllerObj.remove("scale");
//                controllerObj.insert("scale", scale);
//            }
        }
    }

    retObj.insert("property", QJsonValue(propertyName));
    retObj.insert("type", QJsonValue(controllerType));
    if (!controllerObj.isEmpty())
        retObj.insert("controller", QJsonValue(controllerObj));

    return retObj;
}



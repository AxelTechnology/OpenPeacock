#include "metadata.h"

#include <QPainter>
#include <QJsonObject>
#include <logger.h>
#include <QPropertyAnimation>
#include <QtDebug>
#include <QJsonArray>
#include "jsonserializer.h"


Metadata::Metadata(QWidget *parent)
    : AbstractWidget(parent)
{
    _paintWidth = 50;
    _paintHeight = 50;
    _bind = 0;

    configure();
}


void Metadata::configure()
{
    QImage image(":/Metadata.png");
    _iconImage = QImage(image.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    update();
}

Metadata::~Metadata()
{
}




void Metadata::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    AbstractWidget::PainterSetup(&painter);

    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.drawImage(0,0, _iconImage);
    //painter.setWindow(0,0, _paintWidth, _paintHeight);

    //Debug background
//    painter.setBrush(Qt::blue);
//    painter.drawRect(0,0,width(),height());

//    if(_imageLabel != 0)
//        return;

//    QImage* showImage = NULL;
//    if (_showImageIndex < _imageObjList->count())
//        showImage = _imageObjList->at(_showImageIndex);
//    else if(_imageObjList->count() > 0)
//        showImage = _imageObjList->at(0);

//    if (showImage != 0)
//        painter.drawImage(width() / 2 - showImage->width() / 2, height() / 2 - showImage->height() / 2 , *showImage);


//    showImage = NULL;
}

QSize Metadata::sizeHint() const
{
    return QSize(50, 50);
}


void Metadata::resizeEvent(QResizeEvent*)
{

}

void Metadata::mouseReleaseEvent(QMouseEvent *)
{

}

void Metadata::mousePressEvent(QMouseEvent *)
{

}

bool Metadata::ManageRemoteCommand(QString propertyName, QJsonValue value, bool init, QString * error)
{
    Q_UNUSED(error);

    bool retVal = AbstractWidget::ManageRemoteCommand(propertyName, value, init, error);

    return retVal;
}


void Metadata::SetJsonData(QJsonObject *data)
{
    if (data == 0)
        return;

    AbstractWidget::SetJsonData(data);

    _bind = (*data)["bind"].toInt();

}


QJsonObject* Metadata::GetJsonData()
{
    QJsonObject* retObj = AbstractWidget::GetJsonData();

    retObj->insert("bind", QJsonValue(_bind));

    return retObj;
}

QJsonObject *Metadata::GetJsonDataVars()
{    
    QJsonObject* retObj = AbstractWidget::GetJsonDataVars();

    QJsonArray properties;
    //IGNORING COMMON PROPERTIES
//    if(retObj->contains("properties"))
//        properties = (*retObj)["properties"].toArray();
//    else
//        properties = QJsonArray();

    QJsonObject value = JsonSerializer::GetVarsProperty(QString(), "INTEGER", "bind");
    if(!value.isEmpty())
        properties.append(value);

    if(properties.isEmpty())
        return 0;

    retObj->insert("properties", properties);
    return retObj;
}



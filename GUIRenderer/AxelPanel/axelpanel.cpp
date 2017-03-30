#include "axelpanel.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>
#include <QThread>
#include <logger.h>
#include "jsonserializer.h"

AxelPanel::AxelPanel(QWidget *parent)
    : AbstractWidget(parent)
{
    //setMinimumHeight(40);
    //setMinimumWidth(40);
    _colorBg = QColor(50,50,50);
    _colorBlack = QColor(Qt::black);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(Blink()));


}

AxelPanel::~AxelPanel()
{
    timer->stop();
    delete timer;
}


void AxelPanel::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    AbstractWidget::PainterSetup(&painter);
    painter.setRenderHint(QPainter::Antialiasing, false);

    //painter.setBrush(_colorBg);
    QColor bg = _colorBg;
    painter.fillRect(0,0,width(),height(), bg);
}



bool AxelPanel::ManageRemoteCommand(QString propertyName, QJsonValue value, bool init, QString * error)
{
    Q_UNUSED(error);

    QString objName = this->objectName();
    Logger::Instance()->AddLog( objName + ": Got remove command", LoggerLevel_DEBUGVERBOSE);

    bool retVal = AbstractWidget::ManageRemoteCommand(propertyName, value, init, error);

    if(propertyName == "colorBg")
    {
        _colorBg = QColor(value.toString());
        update();
    }
    if(propertyName == "BLINK")
    {
        int delay = value.toInt(50);
        _colorBg_bk = _colorBg;
        elapsedTimer.start();
        timer->start(delay);
    }

    return retVal;
}

void AxelPanel::Blink()
{
    if (elapsedTimer.elapsed() > 10000)
    {
        timer->stop();
        _colorBg = _colorBg_bk;
        update();
        return;
    }

    if(isBlack)
    {
        _colorBg = _colorBg_bk;
        isBlack = false;
    }
    else
    {
        _colorBg = _colorBlack;
        isBlack = true;
    }

    update();

}


void AxelPanel::SetJsonData(QJsonObject *data)
{
    if (data == 0)
        return;
    AbstractWidget::SetJsonData(data);
    _colorBg = GetColorFromString((*data)["colorBg"].toString());

}

QJsonObject* AxelPanel::GetJsonData()
{
    QJsonObject* retObj = AbstractWidget::GetJsonData();
    retObj->insert("colorBg", QJsonValue(GetStringFromColor(_colorBg)));
    return retObj;
}

QJsonObject *AxelPanel::GetJsonDataVars()
{
    QJsonObject* retObj = AbstractWidget::GetJsonDataVars();

    if((retObj == 0) || (!retObj->contains("properties")))
        return 0;

    return retObj;
}



#include "abstractwidget.h"
#include <QtGlobal>
#include <QJsonObject>
#include <QJsonArray>
#include <jsonserializer.h>

AbstractWidget::AbstractWidget(QWidget* parent) : QWidget(parent)
{
    connect(this, SIGNAL(objectNameChanged(QString)), this, SLOT(slotObjectNameChanged(QString)));
    _parentObjectName = QString();
    _visible = true;

    _autoHideMsec = 0;
    _elapsedTimerAutoHide.start();

    _timerAutoHide = new QTimer(this);
    connect(_timerAutoHide, SIGNAL(timeout()), this, SLOT(timerAutoHideTick()));
    _timerAutoHide->start(50);

    _animation = new QPropertyAnimation(this, "geometry");
    _animation->setDuration(1000);
    _animation->setEasingCurve(QEasingCurve::InOutExpo);
    connect(_animation, SIGNAL(finished()), this, SLOT(AnimationFinished()));
    _animationStarted = false;
    _animationEnabled = false;
    _animationEndGeometry = QRect(0, 0, 0, 0);
    _animationTimer = new QTimer(this);
    _animationTimer->setInterval(2000);
    _animationTimer->setSingleShot(true);
    connect(_animationTimer, SIGNAL(timeout()), this, SLOT(EndAnimation()));
}

AbstractWidget::~AbstractWidget()
{
    _timerAutoHide->stop();
    delete _timerAutoHide;
    _animationTimer->stop();
    delete _animationTimer;
}

void AbstractWidget::slotObjectNameChanged(QString objectName)
{
    _parentObjectName = QString();
    if(objectName.isNull() || objectName.isEmpty())
        return;
}

void AbstractWidget::setWidgetVisible(bool value)
{
    _visible = value;
    if(_visible)
    {
        this->show();
        _elapsedTimerAutoHide.restart();
    }
    else
        this->hide();
}

bool AbstractWidget::ManageRemoteCommand(QString propertyName, QJsonValue value, bool init, QString *)
{
    if(propertyName == "visible")
    {
        int val = value.toInt(0);
        if(val == 0)
            setVisible(false);
        else
            setVisible(true);
    }
    if(propertyName == "enabled")
    {
        if(value.toInt(0) == 0)
            setEnabled(false);
        else
            setEnabled(true);
    }
    return true;
}


void AbstractWidget::SetJsonData(QJsonObject *data)
{
    if (data == 0)
        return;
    this->setObjectName((*data)["id"].toString());
    _parentObjectName = (*data)["parentObjectName"].toString();    
    _visible = (*data)["visible"].toInt();
    this->setEnabled((*data)["enabled"].toInt());
    this->setAutoHideMsec((*data)["autoHideMsec"].toInt());
    this->setAnimationEnabled((*data)["animationEnabled"].toInt());
    this->setAnimationEndGeometry(QRect((*data)["animationX"].toInt(), (*data)["animationY"].toInt(), (*data)["animationWidth"].toInt(), (*data)["animationHeight"].toInt()));
    _animationStartGeometry = this->geometry();
}


QJsonObject* AbstractWidget::GetJsonData()
{
    QJsonObject *objRet = new QJsonObject();

    objRet->insert("type", QJsonValue(this->metaObject()->className()));
    objRet->insert("id", QJsonValue(this->objectName()));
    objRet->insert("parentObjectName", QJsonValue(_parentObjectName));
    objRet->insert("x", QJsonValue(this->geometry().x()));
    objRet->insert("y", QJsonValue(this->geometry().y()));
    objRet->insert("width", QJsonValue(this->geometry().width()));
    objRet->insert("height", QJsonValue(this->geometry().height()));    

    if(this->isEnabled())
        objRet->insert("enabled", QJsonValue(1));
    else
        objRet->insert("enabled", QJsonValue(0));


    if(_visible)
        objRet->insert("visible", QJsonValue(1));
    else
        objRet->insert("visible", QJsonValue(0));

    objRet->insert("autoHideMsec", QJsonValue(_autoHideMsec));

    objRet->insert("animationX", _animationEndGeometry.x());
    objRet->insert("animationY", _animationEndGeometry.y());
    objRet->insert("animationWidth", _animationEndGeometry.width());
    objRet->insert("animationHeight", _animationEndGeometry.height());

    if(_animationEnabled)
        objRet->insert("animationEnabled", QJsonValue(1));
    else
        objRet->insert("animationEnabled", QJsonValue(0));

    return objRet;
}


QJsonObject* AbstractWidget::GetJsonDataVars()
{
    QJsonObject *objRet = new QJsonObject();
    //objRet->insert("id", QJsonValue(_id));
    objRet->insert("id", QJsonValue(objectName()));

    QJsonArray properties = QJsonArray();

    QJsonObject value = JsonSerializer::GetVarsProperty(m_controllerVisibleId, "INTEGER", "visible");
    if(!value.isEmpty())
        properties.append(value);

    value = JsonSerializer::GetVarsProperty(m_controllerEnabledId, "INTEGER", "enabled");
    if(!value.isEmpty())
        properties.append(value);

    if(!properties.isEmpty())
        objRet->insert("properties", properties);

    return objRet;
}



QString AbstractWidget::GetStringFromColor(QColor color)
{
    QString rgb = QString(color.name());
    QString alpha = QString::number(color.alpha(), 16);
    if (alpha.length() < 2)
        alpha = "0" + alpha;

    return rgb + alpha;
}

QColor AbstractWidget::GetColorFromString(QString color)
{
    QColor retVal;
    if(color.isNull() || color.isEmpty())
        retVal = QColor();
    else if(color.length() < 8)
        retVal = QColor(color);
    else if(color.length() >= 9)
    {
        QString rgb = color.mid(0, 7);
        QString alpha = color.mid(7, 2);
        retVal = QColor(rgb);
        retVal.setAlpha(alpha.toInt(0,16));
    }
    else
        retVal = QColor();

    return retVal;
}


void AbstractWidget::PainterSetup(QPainter* painter)
{
    if (painter == 0)
        return;

    if (!this->isEnabled())
        painter->setOpacity(0.5);

}


void AbstractWidget::timerAutoHideTick()
{
    if(_autoHideMsec <= 0)
        return;

    if (_elapsedTimerAutoHide.elapsed() < _autoHideMsec)
        return;

    this->hide();
}



void AbstractWidget::resizeEvent(QResizeEvent *)
{
}



void AbstractWidget::StartAnimation()
{    
    if(!_animationEnabled)
        return;
    //if(_animation->state() == QAbstractAnimation::Running)
    //    return;
    if (_animationStarted)
    {
        _animationTimer->stop();
    }
    else
    {
        _animationStarted = true;
        _animation->setStartValue(this->geometry()); // _animationStartGeometry);
        _animation->setEndValue(_animationEndGeometry);
        _animation->start();
    }
    _animationTimer->start();
}


void AbstractWidget::EndAnimation()
{    
    _animation->setEndValue(_animationStartGeometry);  //_animation->startValue());
    _animation->setStartValue(_animationEndGeometry); // _animationEndGeometry);
    _animation->start();
    _animationStarted = false;
    AnimationStarted();
}

void AbstractWidget::AnimationFinished()
{
}

void AbstractWidget::AnimationStarted()
{
}

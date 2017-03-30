#include "vumeter.h"


#include <QFontDatabase>
#include <QJsonObject>
#include <QFileInfo>
#include <QJsonArray>
#include <jsonserializer.h>
#include <QResizeEvent>


VuMeter::VuMeter(QWidget *parent)
    : AbstractWidget(parent)
{
    _paintWidth = 50;
    _paintHeight = 200;

    _valueA = 0;
    _valueAPerc = 0;
    _paintValueA = 0;
    _valueB = 0;
    _valueBPerc = 0;
    _paintValueB = 0;
    _maxValue = 10;
    _color1MaxValue = 0;
    _color2MaxValue = 0;

    _numVuMeters = 2;
    _showLabelsLeft = false;

    _color1 = QColor(255, 255, 255);
    _color2 = QColor(255, 255, 255);
    _color3 = QColor(255, 255, 255);
    _ledAttenuation = 0.2;
    _color1off = QColor(255 * _ledAttenuation, 255 * _ledAttenuation, 255 * _ledAttenuation);
    _color2off = QColor(255 * _ledAttenuation, 255 * _ledAttenuation, 255 * _ledAttenuation);
    _color3off = QColor(255 * _ledAttenuation, 255 * _ledAttenuation, 255 * _ledAttenuation);

    m_decayValuePerSec = 20;

    _elapsedTimerA.start();
    _elapsedTimerB.start();



    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimerTick()));
    timer->start(50);

}

VuMeter::~VuMeter()
{
    timer->stop();
    delete timer;
}


void VuMeter::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    AbstractWidget::PainterSetup(&painter);
    //painter.setRenderHint(QPainter::Antialiasing, false);

    //Debug background
    //painter.setBrush(Qt::blue);
    //painter.drawRect(0,0,_paintWidth,_paintHeight);


    int ledMargin = 2;
    int ledHeigth = (_paintHeight - (ledMargin * (_maxValue -1))) / _maxValue;

    int marginX = _paintWidth * 0.05;

    int tickWidth = _paintWidth / 2 - marginX / 2 - _labelMargin / 2;

    int tick1X = (_showLabelsLeft ? _labelMargin : 0);
    int tick2X = 0;
    if(_numVuMeters == 1)    
        tick1X = _paintWidth / 2 - tickWidth / 2; // + (_showLabelsLeft ? _labelMargin : 0);
    else
        tick2X = tickWidth + marginX + (_showLabelsLeft ? _labelMargin : 0);

    painter.setPen(QColor(254, 254, 254));

    for (int k = 0; k < _labels.count(); k++)
    {
        int y = k * ((_paintHeight - _labelMargin) / (_labels.count() - 1));
        //int x = _paintWidth - _labelMargin; // tick1X + tick2X + tickWidth + marginX;
        //QRectF txtRect = QRectF(x, y, _labelMargin, _labelMargin);
        //Debug labels
        //painter.setBrush(Qt::red);
        //painter.drawRect(txtRect);
        //drawText(painter, setWidth, y, Qt::AlignRight, _labels.at(k), &txtRect, &_labelsFont);
        painter.setFont(_labelsFont);

        if(_showLabelsLeft)
            drawText(painter, _labelMargin - 3, y + _labelMargin/3 , Qt::AlignTop | Qt::AlignRight, _labels.at(k));
        else
            drawText(painter, _paintWidth, y + _labelMargin/3 , Qt::AlignTop | Qt::AlignRight, _labels.at(k));
    }


    for (int i = 1; i <= _maxValue; i++)
    {
        QColor ledColorAon;
        QColor ledColorBon;
        QColor ledColorAoff;
        QColor ledColorBoff;

        if((_color1MaxValue <= 0) && (_color2MaxValue <= 0))
        {
            ledColorAon = _color1;
            ledColorBon = _color1;
            ledColorAoff = _color1off;
            ledColorBoff = _color1off;
        }
        else if (i <= _color1MaxValue)
        {
            ledColorAon = _color1;
            ledColorBon = _color1;
            ledColorAoff = _color1off;
            ledColorBoff = _color1off;
        }
        else if(i <= _color2MaxValue)
        {
            ledColorAon = _color2;
            ledColorBon = _color2;
            ledColorAoff = _color2off;
            ledColorBoff = _color2off;
        }
        else
        {
            ledColorAon = _color3;
            ledColorBon = _color3;
            ledColorAoff = _color3off;
            ledColorBoff = _color3off;
        }

        int ledY = _paintHeight - (ledHeigth * i) - (ledMargin * (i-1));

        if(_numVuMeters == 1)
        {
            if(i <= _paintValueA)
                painter.setBrush(ledColorAon);
            else
                painter.setBrush(ledColorAoff);
            painter.fillRect(tick1X, ledY, tickWidth, ledHeigth, painter.brush()); //, painter.brush() );
        }
        else
        {
            if(i <= _paintValueA)
                painter.setBrush(ledColorAon);
            else
                painter.setBrush(ledColorAoff);
            painter.fillRect(tick1X, ledY, tickWidth, ledHeigth, painter.brush() );
            if(i <= _paintValueB)
                painter.setBrush(ledColorBon);
            else
                painter.setBrush(ledColorBoff);
            painter.fillRect(tick2X, ledY, tickWidth, ledHeigth, painter.brush() );
        }
    }
}

/*
 * When you pass a starting point for drawing text, you're effectively drawing text on an a large rectangle
 * that has the bottom-left corner at the given point. So all you need is to offer a suitable "infinite"
 * rectangle based on your starting point and the chosen alignment
 * http://stackoverflow.com/questions/24831484/how-to-align-qpainter-drawtext-around-a-point-not-a-rectangle
 */
void VuMeter::drawText(QPainter & painter, qreal x, qreal y, int flags,
              const QString & text, QRectF * boundingRect, QFont * font)
{
    if(boundingRect == 0)
        return;
    if(font == 0)
        return;

    double fontStretch = 1;
    painter.resetTransform();

    if(boundingRect != 0)
    {
        QFontMetrics fm(*font);
        int maxTextWidth = boundingRect->width() - 10;
        int textWidth = fm.width(text);
        if(textWidth > maxTextWidth)
        {
            fontStretch = (double)maxTextWidth / textWidth;
            painter.scale(fontStretch,1);
        }
     }

    const qreal size = 32767.0;
    QPointF corner(x / fontStretch, y - size);
    if (flags & Qt::AlignHCenter) corner.rx() -= size/2.0;
    else if (flags & Qt::AlignRight) corner.rx() -= size;
    if (flags & Qt::AlignVCenter) corner.ry() += size/2.0;
    else if (flags & Qt::AlignTop) corner.ry() += size;
    else flags |= Qt::AlignBottom;
    QRectF rect(corner, QSizeF(size, size));

    painter.drawText(rect, flags, text, boundingRect);

    painter.resetTransform();
}

/*
 * When you pass a starting point for drawing text, you're effectively drawing text on an a large rectangle
 * that has the bottom-left corner at the given point. So all you need is to offer a suitable "infinite"
 * rectangle based on your starting point and the chosen alignment
 * http://stackoverflow.com/questions/24831484/how-to-align-qpainter-drawtext-around-a-point-not-a-rectangle
 */
void VuMeter::drawText(QPainter & painter, qreal x, qreal y, Qt::Alignment flags,
              const QString & text, QRectF * boundingRect)
{
   const qreal size = 32767.0;
   QPointF corner(x, y - size);
   if (flags & Qt::AlignHCenter) corner.rx() -= size/2.0;
   else if (flags & Qt::AlignRight) corner.rx() -= size;
   if (flags & Qt::AlignVCenter) corner.ry() += size/2.0;
   else if (flags & Qt::AlignTop) corner.ry() += size;
   else flags |= Qt::AlignBottom;
   QRectF rect(corner, QSizeF(size, size));
   painter.drawText(rect, flags, text, boundingRect);
}

void VuMeter::updateTimerTick()
{
    if ((_paintValueA <= 0 ) && (_paintValueB <= 0))
        return;

    if (_paintValueA > 0 )
    {
        double decay = ((_elapsedTimerA.elapsed() / 1000.0) * m_decayValuePerSec);
        _paintValueA = _valueA - decay;
    }

    if (_paintValueB > 0 )
    {
        double decay = ((_elapsedTimerB.elapsed() / 1000.0) * m_decayValuePerSec);
        _paintValueB = _valueB - decay;
    }

    update();
}


void VuMeter::setValueA(double val)
{
    if(val < _paintValueA)
        return;

    _valueA = val;
    _paintValueA = _valueA;
    _elapsedTimerA.restart();
}

void VuMeter::setValueAPerc(double valuePerc)
{
    _valueAPerc = valuePerc;
    setValueA(_maxValue * valuePerc / 100);
}

void VuMeter::setValueB(double val)
{
    if(val < _paintValueB)
        return;

    _valueB = val;
    _paintValueB = _valueB;
    _elapsedTimerB.restart();
}

void VuMeter::setValueBPerc(double valuePerc)
{
    _valueBPerc = valuePerc;
    setValueB(_maxValue * valuePerc / 100);
}


void VuMeter::resizeEvent(QResizeEvent* event)
{
    AbstractWidget::resizeEvent(event);    
    _paintWidth = event->size().width();
    _paintHeight = event->size().height();

    _labelMargin = 0;
    if(_labels.count() > 0)
        _labelMargin = _paintWidth * 0.40;
    _labelsFont = QFont("Arial", _labelMargin * 0.45);
}

void VuMeter::mouseReleaseEvent(QMouseEvent *)
{

}

void VuMeter::mousePressEvent(QMouseEvent *)
{

}

bool VuMeter::ManageRemoteCommand(QString propertyName, QJsonValue value, bool init, QString * error)
{
    Q_UNUSED(error);
    bool retVal = AbstractWidget::ManageRemoteCommand(propertyName, value, init, error);

    if(!retVal)
    {
        error = new QString("Error setting VuMeter value " + this->objectName() + " property=" + propertyName + " value=" + value.toString() );
        return retVal;
    }

    if(propertyName == "numVuMeters")
    {
        setNumVuMeters(value.toInt(2));
        return true;
    }

    if (!this->isEnabled())
        return retVal;

    double newValue = value.toDouble(0);

    if (propertyName == "valueA")
        setValueA(newValue);
    else if(propertyName == "valueAPerc")
        setValueAPerc(newValue);
    else if (propertyName == "valueB")
        setValueB(newValue);
    else if(propertyName == "valueBPerc")
        setValueBPerc(newValue);

    return retVal;
}


void VuMeter::setColor1(QColor value)
{
    _color1 = value;
    _color1off = QColor(_color1.red() * 0.2, _color1.green() * 0.2, _color1.blue() * 0.2);

    update();
}

void VuMeter::setColor2(QColor value)
{
    _color2 = value;
    _color2off = QColor(_color2.red() * 0.2, _color2.green() * 0.2, _color2.blue() * 0.2);

    update();
}

void VuMeter::setColor3(QColor value)
{
    _color3 = value;
    _color3off = QColor(_color3.red() * 0.2, _color3.green() * 0.2, _color3.blue() * 0.2);

    update();
}


void VuMeter::SetJsonData(QJsonObject *data)
{
    if (data == 0)
        return;
    AbstractWidget::SetJsonData(data);

    _valueA = (*data)["valueA"].toDouble();
    _valueB = (*data)["valueB"].toDouble();
    _maxValue = (*data)["maxValue"].toDouble();
    _numVuMeters = (*data)["numVuMeters"].toInt();
    QJsonValue values = (*data)["labels"];
    SetJsonLabels(&values);
    _color1MaxValue = (*data)["color1MaxValue"].toDouble();
    _color2MaxValue = (*data)["color2MaxValue"].toDouble();
    setColor1(GetColorFromString((*data)["color1"].toString()));
    setColor2(GetColorFromString((*data)["color2"].toString()));
    setColor3(GetColorFromString((*data)["color3"].toString()));
    m_decayValuePerSec = (*data)["decayMsec"].toInt();

    if((*data)["showLabelsLeft"].toInt() > 0)
        _showLabelsLeft = true;
    else
        _showLabelsLeft = false;

}

bool VuMeter::SetJsonLabels(QJsonValue* values)
{
    if (values == 0)
        return false;

    _labels.clear();

    if((*values).isArray())
    {
        try
        {
            QJsonArray valuesArray = values->toArray();
            if (valuesArray.isEmpty())
                return false;
            foreach (QJsonValue v, valuesArray) {
                if(v.isNull())
                    continue;
                _labels.append(v.toString());
            }
        }
        catch(...)
        {
            return false;
        }
    }
    else
        return false;



    return true;
}


QJsonObject* VuMeter::GetJsonData()
{
    QJsonObject* retObj = AbstractWidget::GetJsonData();
    retObj->insert("valueA", QJsonValue(_valueA));
    retObj->insert("valueB", QJsonValue(_valueA));
    retObj->insert("maxValue", QJsonValue(_maxValue));
    retObj->insert("numVuMeters", QJsonValue(_numVuMeters));
    QJsonArray values = QJsonArray();
    foreach (QString s, _labels) {
        values.append(QJsonValue(s));
    }
    if(!values.isEmpty())
        retObj->insert("labels", QJsonValue(values));
    retObj->insert("showLabelsLeft", QJsonValue(_showLabelsLeft ? 1 : 0));
    retObj->insert("color1MaxValue", QJsonValue(_color1MaxValue));
    retObj->insert("color2MaxValue", QJsonValue(_color2MaxValue));
    retObj->insert("color1", QJsonValue(GetStringFromColor(_color1)));
    retObj->insert("color2", QJsonValue(GetStringFromColor(_color2)));
    retObj->insert("color3", QJsonValue(GetStringFromColor(_color3)));
    //retObj->insert("colorOff", QJsonValue(GetStringFromColor(_colorOff)));
    retObj->insert("decayMsec", QJsonValue(m_decayValuePerSec));
    return retObj;
}

QJsonObject *VuMeter::GetJsonDataVars()
{
    QJsonObject* retObj = AbstractWidget::GetJsonDataVars();
    QJsonArray properties;
    if(retObj->contains("properties"))
        properties = (*retObj)["properties"].toArray();
    else
        properties = QJsonArray();

    QJsonObject value = JsonSerializer::GetVarsProperty(_controllerDataValueA, "DOUBLE", "valueA");
    if(!value.isEmpty())
        properties.append(value);

    value = JsonSerializer::GetVarsProperty(_controllerDataValueAPerc, "DOUBLE", "valueAPerc");
    if(!value.isEmpty())
        properties.append(value);

    value = JsonSerializer::GetVarsProperty(_controllerDataValueB, "DOUBLE", "valueB");
    if(!value.isEmpty())
        properties.append(value);

    value = JsonSerializer::GetVarsProperty(_controllerDataValueBPerc, "DOUBLE", "valueBPerc");
    if(!value.isEmpty())
        properties.append(value);

    value = JsonSerializer::GetVarsProperty(_controllerDataNumVuMeters, "INTEGER", "numVuMeters");
    if(!value.isEmpty())
        properties.append(value);

    if(properties.isEmpty())
        return 0;

    retObj->insert("properties", properties);

    return retObj;
}




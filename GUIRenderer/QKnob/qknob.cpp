
#include "qknob.h"
#include <QtGlobal>
#include <QtGui>
#include <QPainter>
#include <QFont>
#include "logger.h"
#include "jsonserializer.h"


QKnob::QKnob(QWidget *parent)
    : AbstractWidget(parent)
{
    _showValueSign = true;

    m_value = 0;
    m_instantValue = 0;
    m_maxValue = 10;
    m_minValue = -10;

    frameNo = 50;
    diameterCircleBig=100;
    diameterCircleLittle=60;
    degree=1;
    m_labelSize = 0;
    m_stepValue = 0.1;

    knobOffset = -8;

    m_knobMode = QKnobMode_INSTANTVALUE;

    setMinimumSize(QSize(80,80));
    setMaximumSize(QSize(1024,1024));
    setForeground(QColor(253,253,253));
    setBackground(QColor(50, 50, 50));
    setEditingColor(Qt::white);

//    int id = QFontDatabase::addApplicationFont(":/CALIBRI.TTF");
//    if (QFontDatabase::applicationFontFamilies(id).count() <= 0)
//            return;
    //QString fontFamily = "Calibri"; //QFontDatabase::applicationFontFamilies(id).at(0);
    QString fontFamily = "Arial";

    _textFont = new QFont(fontFamily, 15, QFont::Bold);
    _extraTextFont = new QFont(fontFamily, 8, QFont::Bold);

}



QKnob::~QKnob()
{
    //delete ;
}




void QKnob::recalculateData(double value, double instantValue)
{
    m_labelSize = 0;


    if((value > -0.05) && (value < 0.05))
        m_value = 0;
    else
        m_value=value;

    if((instantValue > -0.05) && (instantValue < 0.05))
        m_instantValue = 0;
    else
        m_instantValue = instantValue;

    if(m_instantValue==0){
        degree=1;
    }

    if(m_value > m_maxValue)
        m_value = m_maxValue;
    else if(m_value < m_minValue)
        m_value = m_minValue;

    if(m_instantValue > m_maxValue)
        m_instantValue = m_maxValue;
    else if(m_instantValue < m_minValue)
        m_instantValue = m_minValue;


    Logger::Instance()->AddLog("QKnob: set value 1", LoggerLevel_DEBUG);

    if( m_instantValue>0.0){
         degree=(145/m_maxValue)*m_instantValue;
         degree=-degree;         
    } else if (m_instantValue<0.0){
         degree=-((145/m_maxValue)*m_instantValue);
    }

    text = QString::number(m_value,'f',1) + m_units;
    instantText = QString::number( m_instantValue,'f',1);

    if(_showValueSign)
    {
        if((!text.startsWith("-")) && (m_value != 0))
            text = "+" + text;
        if((!instantText.startsWith("-")) && (m_instantValue != 0))
            instantText = "+" + instantText;
    }
    else
    {
        if(text.startsWith("-"))
            text = text.replace("-","");
        if(instantText.startsWith("-"))
            instantText = instantText.replace("-","");
    }

    if(m_leftLabel.isEmpty())
        m_leftLabelDraw = QString::number(m_minValue,'f',0);
    else
        m_leftLabelDraw = m_leftLabel;

    if(m_rightLabel.isEmpty())
        m_rightLabelDraw = QString::number(m_maxValue,'f',0);
    else
        m_rightLabelDraw = m_rightLabel;

    Logger::Instance()->AddLog("QKnob: set value 2", LoggerLevel_DEBUG);

}

void QKnob::setValue(double value)
{
    recalculateData(value, value);
    update();
}

void QKnob::setInstantValue(double instantValue)
{
    recalculateData(m_value, instantValue);
    update();
}




void QKnob::setMinValue(double value)
{
   m_minValue=value;
   recalculateData(m_value, m_value);
   update();
}



void QKnob::setMaxValue(double value)
{
    if(value > m_minValue)
    {
        m_maxValue=value;
    }
    else
        emit errorSignal(MaxValueError);

    recalculateData(m_value, m_value);
}





void QKnob::setUnits(QString units)
{
    m_units=units;
    recalculateData(m_value, m_value);
    update();
}

void QKnob::setLeftLabel(QString leftLabel)
{
    m_leftLabel = leftLabel;
    recalculateData(m_value, m_value);
    update();
}

void QKnob::setRightLabel(QString rightLabel)
{
    m_rightLabel = rightLabel;
    recalculateData(m_value, m_value);
    update();
}


void QKnob::resizeEvent(QResizeEvent *event)
{
    AbstractWidget::resizeEvent(event);
    Q_UNUSED(event);
    update();
}


void QKnob::changeEvent(QEvent*)
{
        //setValue(value());
}


void QKnob::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    AbstractWidget::PainterSetup(&painter);
    painter.setRenderHint(QPainter::Antialiasing, true);


    int side = qMin(width(), height());
    painter.setViewport((width() - side) / 2, (height() - side) / 2,side, side);
    painter.setWindow(-60, -60, 120, 120);

    //Debug background
    //painter.fillRect(-60,-60 ,120,120, Qt::blue);

    if(m_value == m_instantValue)
        editingValue = false;
    else
        editingValue = true;

    setClipping(&painter);
    drawCircleOutside(&painter);
    drawArea(&painter);
    //drawCircleInternal(&painter);
    //drawTriangle(&painter);

    painter.setClipping(false);    
    drawLabelBg(&painter);
    drawLabel(&painter, _textFont, _extraTextFont);

}


void QKnob::setForeground(QColor newForeColor)
{
    m_foreground=newForeColor;
    update();
}


void QKnob::setBackground(QColor newBackColor)
{
    m_background=newBackColor;
    update();
}


bool QKnob::ManageRemoteCommand(QString propertyName, QJsonValue value, bool init, QString * error)
{
    Q_UNUSED(error);
    bool retVal = AbstractWidget::ManageRemoteCommand(propertyName, value, init, error);

    if(!retVal)
        error = new QString("Error setting QKnob value " + this->objectName() + " property=" + propertyName + " value=" + value.toString() );

    if (!this->isEnabled())
        return retVal;


    double knobValue = value.toDouble(0);    

    if(propertyName == "value")
        recalculateData(knobValue, knobValue);
    else if(propertyName == "instantValue")
        recalculateData(m_value, knobValue);
    else if(propertyName == "minValue")
        setMinValue(knobValue);
    else if(propertyName == "maxValue")
        setMaxValue(knobValue);
    update();


    return retVal;
}


void QKnob::SetJsonData(QJsonObject *data)
{
    if (data == 0)
        return;
    AbstractWidget::SetJsonData(data);

    m_minValue = (*data)["minValue"].toInt();
    m_maxValue = (*data)["maxValue"].toInt();
    m_units = (*data)["unitText"].toString();
    m_value = ((*data)["value"].toDouble());
    //m_instantValue = ((*data)["instantValue"].toDouble());
    m_stepValue = ((*data)["stepValue"].toDouble());
    m_background = GetColorFromString((*data)["bgColor"].toString());
    m_foreground = GetColorFromString((*data)["fgColor"].toString());
    m_editingColor = GetColorFromString((*data)["editingColor"].toString());
    m_leftLabel = (*data)["leftLabel"].toString();
    m_rightLabel = (*data)["rightLabel"].toString();
    _showValueSign = (*data)["showValueSign"].toInt();
    m_knobMode = (QKnobMode) (*data)["knobMode"].toInt();

    recalculateData(m_value, m_value);
}



QJsonObject* QKnob::GetJsonData()
{
    QJsonObject* retObj = AbstractWidget::GetJsonData();

    retObj->insert("minValue", QJsonValue(m_minValue));
    retObj->insert("maxValue", QJsonValue(m_maxValue));
    retObj->insert("unitText", QJsonValue(m_units));
    retObj->insert("value", QJsonValue(m_value));
    //retObj->insert("instantValue", QJsonValue(m_instantValue));
    retObj->insert("stepValue", QJsonValue(m_stepValue));
    retObj->insert("bgColor", QJsonValue(GetStringFromColor(m_background)));
    retObj->insert("fgColor", QJsonValue(GetStringFromColor(m_foreground)));
    retObj->insert("editingColor", QJsonValue(GetStringFromColor(m_editingColor)));
    retObj->insert("leftLabel", QJsonValue(m_leftLabel));
    retObj->insert("rightLabel", QJsonValue(m_rightLabel));
    retObj->insert("knobMode", QJsonValue((int)m_knobMode));

    if(_showValueSign)
        retObj->insert("showValueSign", QJsonValue(1));
    else
        retObj->insert("showValueSign", QJsonValue(0));

    return retObj;
}


QJsonObject *QKnob::GetJsonDataVars()
{
    QJsonObject* retObj = AbstractWidget::GetJsonDataVars();
    QJsonArray properties;
    if(retObj->contains("properties"))
        properties = (*retObj)["properties"].toArray();
    else
        properties = QJsonArray();

    QJsonObject value = JsonSerializer::GetVarsProperty(_controllerDataValue, "DOUBLE", "value");
    if(!value.isEmpty())
        properties.append(value);

    value = JsonSerializer::GetVarsProperty(QString(), "DOUBLE", "minValue");
    if(!value.isEmpty())
        properties.append(value);
    value = JsonSerializer::GetVarsProperty(QString(), "DOUBLE", "maxValue");
    if(!value.isEmpty())
        properties.append(value);


    if(m_knobMode == QKnobMode_INSTANTVALUE)
    {
        value = JsonSerializer::GetVarsProperty(_controllerDataInstantValue, "DOUBLE", "instantValue");
        if(!value.isEmpty())
            properties.append(value);
    }

    if(properties.isEmpty())
        return 0;

    retObj->insert("properties", properties);
    return retObj;
}

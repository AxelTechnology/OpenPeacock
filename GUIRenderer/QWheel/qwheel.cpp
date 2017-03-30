/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "qwheel.h"
#include "logger.h"
#include <jsonserializer.h>

#define WHEEL_SCROLL_OFFSET 50000.0

QWheel::QWheel(QWidget *parent,WheelConfiguration *configuration)
    : AbstractWidget(parent)    
    , m_itemOffset(1)
{

    _newItem = 0;
    m_currentItem = 0;
    m_instantValue = 0;
    m_lastReceivedInstantValue = 0;
    m_lastReceivedValue = 0;
    m_selectedItems.clear();
    m_wheelMode = QWheelMode_SINGLESELECTION;

    setSelectedColor(QColor(253,253,0));
    setUnselectedColor(QColor(253,253,253));

    m_backgroundWheel = QColor(90,90,90);
    _unselectedColor = Qt::white;

    m_items << "OFF" << "ON";

    if (configuration != 0)
    {
        QRect geometry = *configuration->position->pos;
        this->setGeometry(geometry);
        configure(*configuration);
    }

    QScroller::grabGesture(this,  QScroller::LeftMouseButtonGesture);

//    int id = QFontDatabase::addApplicationFont(":/CALIBRI.TTF");
//    if (QFontDatabase::applicationFontFamilies(id).count() <= 0)
//            return;
//    QString fontFamily = QFontDatabase::applicationFontFamilies(id).at(0);
    //QString fontFamily = "Calibri";
    QString fontFamily = "Arial";
    this->setFont(QFont(fontFamily, 18, QFont::Bold));

    _initializing = true;

    //multiselection test
//    m_selectedItems.append(0);
//    m_selectedItems.append(0);
//    m_selectedItems.append(1);
//    m_selectedItems.append(0);
//    m_selectedItems.append(1);
//    m_selectedItems.append(1);
//    m_selectedItems.append(0);
//    m_selectedItems.append(0);

}




QWheel::~QWheel()
{ }



void QWheel::updateData(WheelData &data){

    setCurrentIndex(data.value);
}

void QWheel::configure(WheelConfiguration &configuration){

    setItems( configuration.values );
}

bool QWheel::event(QEvent *e)
{
    switch (e->type()) {
// ![1]
    case QEvent::ScrollPrepare:
    {
        // We set the snap positions as late as possible so that we are sure
        // we get the correct itemHeight
        QScroller *scroller = QScroller::scroller(this);
        scroller->setSnapPositionsY( WHEEL_SCROLL_OFFSET, itemHeight() );

        QScrollPrepareEvent *se = static_cast<QScrollPrepareEvent *>(e);
        se->setViewportSize(QSizeF(size()));
        // we claim a huge scrolling area and a huge content position and
        // hope that the user doesn't notice that the scroll area is restricted
        se->setContentPosRange(QRectF(0.0, 0.0, 0.0, WHEEL_SCROLL_OFFSET * 2));
        se->setContentPos(QPointF(0.0, WHEEL_SCROLL_OFFSET + m_instantValue * itemHeight()+1)); // + m_itemOffset));
        se->accept();
        return true;
    }
// ![1]
// ![2]

    case QEvent::Scroll:
    {
        QScrollEvent *se = static_cast<QScrollEvent *>(e);

        qreal y = se->contentPos().y();        
        int ih = itemHeight();        
        int iy = y - WHEEL_SCROLL_OFFSET + ih/2;

// ![2]
// ![3]
        // -- calculate the current item position and offset and redraw the widget
        int ic = itemCount();
        if (ic>0) {
            _scrolling = true;
            //Logger::Instance()->AddLog("WHEEL SCROLL 1: ih=" + QString::number(ih) + " iy=" + QString::number(iy), LoggerLevel_INFO);
            m_instantValue = iy / ih % ic;
            m_itemOffset = iy % ih;

            //Logger::Instance()->AddLog("WHEEL SCROLL 1: m_itemOffset=" + QString::number(m_itemOffset) + " m_instantValue=" + QString::number(m_instantValue), LoggerLevel_INFO);

            // take care when scrolling backwards. Modulo returns negative numbers
            if (m_itemOffset < 0) {
                m_itemOffset += ih;                
                m_instantValue--;
            }

            //Logger::Instance()->AddLog("WHEEL SCROLL 2: m_itemOffset=" + QString::number(m_itemOffset) + " m_instantValue=" + QString::number(m_instantValue), LoggerLevel_INFO);

            if (m_instantValue < 0)
            {
                m_instantValue += ic;
            }
        }

        //Logger::Instance()->AddLog("Scrolling... itemOffset=" + QString::number(m_itemOffset) + " instantValue=" + QString::number(m_instantValue));

        //m_currentItem = m_instantValue;
        // -- repaint
        _scrolling = false;
        update();



        se->accept();
        return true;
// ![3]
    }
    default:
        return QWidget::event(e);
    }
    return true;
}


void QWheel::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    if(m_currentItem == m_instantValue)
        m_editingValue = false;
    else
        m_editingValue = true;

    QColor bufSelectedColor = _selectedColor;
    QColor bufSelectedColorShadowed = _selectedColorShadowed;

    //Paint all the same in string build mode
    if(m_wheelMode == QWheelMode_STRINGBUILD)
    {
        bufSelectedColor = _unselectedColor;
        bufSelectedColorShadowed = _unselectedColorShadowed;
    }

    QPainter painter(this);
    AbstractWidget::PainterSetup(&painter);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QFontMetrics fm(this->font());

    int w = width();
    int h = height();

    // -- first calculate size and position.
    int bottomLabelHeigth = itemHeight();
    int wWheel = w;
    int hWheel = h - bottomLabelHeigth - 4;


    //Debug background
    //painter.fillRect(0, 0, w, h, Qt::blue);

    painter.setWindow(QRect(-wWheel/2,0,w,h));

    int wheelWidth = wWheel-wWheel*0.2;

    QPalette palette = QApplication::palette();
    QPalette::ColorGroup colorGroup = isEnabled() ? QPalette::Active : QPalette::Disabled;

    QBrush gBrush(m_backgroundWheel);

    // paint a border and background
    painter.setPen(palette.color(colorGroup, QPalette::ButtonText));
    painter.setBrush(gBrush);
    // painter.setBrushOrigin( QPointF( 0.0, 0.0 ) );

    QPainterPath wheelPath = QPainterPath(QPoint(-wWheel/2,0));
    wheelPath.addRect(QRect (-wheelWidth/ 2, 0, wheelWidth, hWheel));
    painter.drawPath(wheelPath);




    // paint the items
    painter.setClipPath(wheelPath);
    QColor penColor = QColor("black");
    painter.setPen(penColor);

   int iH = itemHeight();
   int iC = itemCount();


   //paint a square bar over the selected item
   painter.setBrush(Qt::black);


   if (iC > 0)
   {
       int buffOffset = m_itemOffset % iH;

       int minItem = -hWheel/2/iH;
       int maxItem = hWheel/2/iH + 1;

       for (int i=minItem; i<= maxItem; i++) {
            int itemNum = m_instantValue + i;
            if (itemNum < 0)
               continue;
            if (itemNum >= iC)
               continue;

            while (itemNum < 0)
               itemNum += iC;
            while (itemNum >= iC)
               itemNum -= iC;

            int itemYCenter = hWheel/2 + i*iH - buffOffset;

            int radius = hWheel / 2;
            int itemDistance = 0;

            itemDistance = qAbs(radius - itemYCenter);
            if (itemDistance > radius)
                itemDistance = radius;

            bool isCentralItem = false;
            if(i == 0)
                isCentralItem = true;

            bool isSelected = false;
            if ((m_wheelMode == QWheelMode_SINGLESELECTION) || (m_wheelMode == QWheelMode_SINGLESELCTION_INSTANT_ONLY))
            {
                if(itemNum == m_currentItem)
                    isSelected = true;
            }
            else if(m_wheelMode == QWheelMode_MULTISELECTION)
            {
                if(itemNum < m_selectedItems.count())
                    if(m_selectedItems.at(itemNum) > 0)
                        isSelected = true;
            }

            if(isCentralItem)
            {
                if(isSelected)
                    painter.setPen(_selectedColor);
                else
                    painter.setPen(_unselectedColor);
                painter.drawRect(QRect(-wheelWidth/2 + 2, hWheel/2 - iH / 2,  wheelWidth - 4, iH ));
            }

            painter.save();
            painter.translate(0, itemYCenter + iH/2);

            int maxTextWidth = wheelWidth - 10;
            int textWidth = fm.width(m_items.at(itemNum));
            if(textWidth > maxTextWidth)
                painter.scale((double)maxTextWidth / textWidth,1);



            if(isCentralItem) //central item
            {
                if(isSelected)  //item is selected
                    painter.setPen(bufSelectedColor);
                else
                    painter.setPen(_unselectedColor);
            }
            else //all other items
            {
                if(isSelected) //item is selected
                    painter.setPen(bufSelectedColorShadowed);
                else
                    painter.setPen(_unselectedColorShadowed);
            }

            painter.drawText(QRect(-textWidth/ 2, -iH / 2, textWidth, iH), Qt::AlignCenter, m_items.at(itemNum));

            painter.restore();
        }
    }

   painter.setClipping(false);

   //Draw an indicator of the selected item
   painter.setPen(_selectedColor);
   painter.fillRect(QRect(-wheelWidth/2, h - bottomLabelHeigth,  wheelWidth, bottomLabelHeigth ), _unselectedColorShadowed);
   int maxTextWidth = wheelWidth - 10;

   QString selItemLabel;
   if((m_wheelMode == QWheelMode_SINGLESELECTION) || (m_wheelMode == QWheelMode_SINGLESELCTION_INSTANT_ONLY))
       selItemLabel = m_items.at(m_currentItem);
   else if (m_wheelMode == QWheelMode_MULTISELECTION)
   {
       int itemNum = 0;
       foreach (int iSel, m_selectedItems) {
           if (iSel > 0)
               itemNum ++;
       }
       selItemLabel = "SELECTED: " + QString::number(itemNum);
   }
   else if(m_wheelMode == QWheelMode_STRINGBUILD)
   {
       selItemLabel = m_builtInput;
   }


   int textWidth = fm.width(selItemLabel);
   if(textWidth > maxTextWidth)
       painter.scale((double)maxTextWidth / textWidth,1);
   painter.drawText(QRect(-textWidth/2, h - bottomLabelHeigth,  textWidth, iH ), Qt::AlignCenter, selItemLabel);

}


/*!
    Rotates the wheel widget to a given index.
    You can also give an index greater than itemCount or less than zero in which
    case the wheel widget will scroll in the given direction and end up with
    (index % itemCount)
*/

void QWheel::scrollTo()
{    
    if(!_initializing && (_newItem == m_instantValue))
        return;
    _initializing = false;
    QScroller *scroller = QScroller::scroller(this);
//    if(scroller->state() == scroller->Scrolling)
//        scroller->stop();

    //scroller->scrollTo(QPointF(0, WHEEL_SCROLL_OFFSET + index * itemHeight()), 3000);
    scroller->scrollTo(QPointF(0, WHEEL_SCROLL_OFFSET + _newItem * itemHeight()), 100);

}


void QWheel::setValue(int index)
{
    _newItem = index;
    m_currentItem = index;
    QTimer::singleShot(1, this, SLOT(scrollTo()));
    update();
}

void QWheel::setInstantValue(int instantIndex)
{    
    _newItem = instantIndex;
    QTimer::singleShot(1, this, SLOT(scrollTo()));
    update();
}




QStringList QWheel::items() const
{
    return m_items;
}

void QWheel::setItems( const QStringList &items )
{
    m_items = items;
    if(m_currentItem != m_lastReceivedValue)
    {
        if(m_lastReceivedValue >= items.count())
            m_currentItem = items.count()-1;
        else
            m_currentItem = m_lastReceivedValue;
    }
    else
    {
        if (m_currentItem >= items.count())
        {
            if(m_currentItem >= items.count())
                m_currentItem = items.count()-1;
        }
    }
    //update();
    setValue(m_currentItem);
}


QSize QWheel::sizeHint() const
{
    // determine font size
    QFontMetrics fm(font());

    return QSize( fm.width("m") * 10 + 6, fm.height() * 7 + 6 );
}

QSize QWheel::minimumSizeHint() const
{
    QFontMetrics fm(font());

    return QSize( fm.width("m") * 5 + 6, fm.height() * 3 + 6 );
}

void QWheel::paintItem(QPainter* painter, int index, const QRect &rect)
{
    painter->drawText(rect, Qt::AlignCenter, m_items.at(index));
}

int QWheel::itemHeight() const
{
    QFontMetrics fm(font());
    return fm.height();
}

int QWheel::itemCount() const
{
    return m_items.count();
}


bool QWheel::ManageRemoteCommand(QString propertyName, QJsonValue value, bool init, QString * error)
{
    Q_UNUSED(error);
    bool retVal = AbstractWidget::ManageRemoteCommand(propertyName, value, init, error);

    if((propertyName == "value") || (propertyName == "instantValue"))
    {
        if (!this->isEnabled() && !init)
            return retVal;

        int wheelIdx = value.toInt(-1);
        if(wheelIdx == -1)
            retVal = false;

        if(!retVal)
            error = new QString("Error setting QWheel value " + this->objectName() + " property=" + propertyName + " value=" + value.toString() );
        else
        {
            if(propertyName == "value")
            {
                m_lastReceivedValue = wheelIdx;
                setCurrentIndex(wheelIdx);
            }
            else
            {
                m_lastReceivedInstantValue = wheelIdx;
                setInstantValue(wheelIdx);
            }
        }
    }
    else if(propertyName == "values")
    {
        if(!SetJsonValues(&value))
        {
            error = new QString("Error setting QWheel values " + this->objectName() + " property=" + propertyName + " value=" + value.toString() );
            retVal = false;
        }
    }
    else if(propertyName == "selectedValues")
    {
        if (!this->isEnabled())
            return retVal;
        if(!SetJsonSelectedValues(&value))
        {
            error = new QString("Error setting QWheel selected values " + this->objectName() + " property=" + propertyName + " value=" + value.toString() );
            retVal = false;
        }
    }
    else if(propertyName == "builtInput")
    {
        if (!this->isEnabled())
            return retVal;
        setBuiltInput(value.toString());
    }

    return retVal;
}


void QWheel::SetJsonData(QJsonObject *data)
{
    Logger::Instance()->AddLog("Start set wheel data", LoggerLevel_DEBUG);
    if (data == 0)
        return;
    AbstractWidget::SetJsonData(data);

    QJsonValue values = (*data)["values"];
    SetJsonValues(&values);

    QColor col = GetColorFromString((*data)["selectedColor"].toString());
    setSelectedColor(col);
    col = GetColorFromString((*data)["unselectedColor"].toString());
    setUnselectedColor(col);
    m_backgroundWheel = GetColorFromString((*data)["bgWheelColor"].toString());

    m_wheelMode = (QWheelMode)(*data)["wheelMode"].toInt();


    Logger::Instance()->AddLog("Set Value", LoggerLevel_DEBUG);
    setCurrentIndex((*data)["value"].toInt());
    Logger::Instance()->AddLog("End set wheel data", LoggerLevel_DEBUG);

    QJsonValue selValues = (*data)["selectedValues"];
    if(SetJsonSelectedValues(&selValues))
        SetFirstSelectedValue();

}


bool QWheel::SetJsonValues(QJsonValue* values)
{
    if (values == 0)
        return false;

    QStringList items = QStringList();

    if((*values).isArray())
    {
        try
        {
            QJsonArray valuesArray = values->toArray();
            if (valuesArray.isEmpty())
                return false;
            Logger::Instance()->AddLog("Start foreach", LoggerLevel_DEBUGVERBOSE);
            foreach (QJsonValue v, valuesArray) {
                if(v.isNull())
                    continue;
                items.append(v.toString());
            }
            Logger::Instance()->AddLog("End foreach", LoggerLevel_DEBUGVERBOSE);
        }
        catch(...)
        {
            return false;
        }
    }
    else
        return false;

    Logger::Instance()->AddLog("Set Items", LoggerLevel_DEBUG);
    setItems(items);

    return true;
}


bool QWheel::SetJsonSelectedValues(QJsonValue* values)
{
    if (values == 0)
        return false;

    m_selectedItems.clear();    

    if((*values).isArray())
    {
        try
        {
            QJsonArray valuesArray = values->toArray();
            if (valuesArray.isEmpty())
                return false;
            int max = valuesArray.count();
            if (max > m_items.count())
                max = m_items.count();
            for(int i = 0; i < max; i++)
            {
                QJsonValue v = valuesArray.at(i);
                if(v.isNull())
                    continue;
                int iVal = v.toInt();
                m_selectedItems.append(iVal);                
            }
        }
        catch(...)
        {
            return false;
        }
    }
    else
        return false;

    update();


    return true;
}


void QWheel::SetFirstSelectedValue()
{
    if(m_wheelMode != QWheelMode_MULTISELECTION)
        return;
    if(m_selectedItems.isEmpty())
        return;

    int firstSelectedVal = -1;

    for(int i = 0; i < m_selectedItems.count(); i++)
    {
        int iVal = m_selectedItems.at(i);
        if((iVal > 0) && (firstSelectedVal == -1))
            firstSelectedVal = i;
    }

    if(firstSelectedVal != -1)
        setInstantValue(firstSelectedVal);
}


QJsonObject* QWheel::GetJsonData()
{
    QJsonObject* retObj = AbstractWidget::GetJsonData();


    QJsonArray values = QJsonArray();
    foreach (QString s, m_items) {
        values.append(QJsonValue(s));
    }
    retObj->insert("values", QJsonValue(values));
    QJsonArray selValues = QJsonArray();
    foreach (int i, m_selectedItems) {
        selValues.append(QJsonValue(i));
    }
    retObj->insert("selectedValues", QJsonValue(selValues));
    retObj->insert("value", QJsonValue(m_currentItem));

    retObj->insert("selectedColor", QJsonValue(GetStringFromColor(_selectedColor)));
    retObj->insert("unselectedColor", QJsonValue(GetStringFromColor(_unselectedColor)));
    retObj->insert("bgWheelColor", QJsonValue(GetStringFromColor(m_backgroundWheel)));
    retObj->insert("wheelMode", QJsonValue((int)m_wheelMode));

    return retObj;
}


QJsonObject *QWheel::GetJsonDataVars()
{
    QJsonObject* retObj = AbstractWidget::GetJsonDataVars();
    QJsonArray properties;
    if(retObj->contains("properties"))
        properties = (*retObj)["properties"].toArray();
    else
        properties = QJsonArray();

    QJsonObject value;

    if((m_wheelMode == QWheelMode_SINGLESELECTION) || (m_wheelMode == QWheelMode_SINGLESELCTION_INSTANT_ONLY))
    {
        value = JsonSerializer::GetVarsProperty(_controllerDataValue, "INTEGER", "value");
        if(!value.isEmpty())
            properties.append(value);
    }
    else if(m_wheelMode == QWheelMode_MULTISELECTION)
    {
        value = JsonSerializer::GetVarsProperty(_controllerDataSelectedValues, "INTARRAY", "selectedValues");
        if(!value.isEmpty())
            properties.append(value);
    }

    value = JsonSerializer::GetVarsProperty(_controllerDataValues, "STRINGARRAY", "values");
    if(!value.isEmpty())
        properties.append(value);

    if(m_wheelMode != QWheelMode_SINGLESELCTION_INSTANT_ONLY)
    {
        value = JsonSerializer::GetVarsProperty(_controllerDataInstantValue, "INTEGER", "instantValue");
        if(!value.isEmpty())
            properties.append(value);
    }

    if(m_wheelMode == QWheelMode_STRINGBUILD)
    {
        value = JsonSerializer::GetVarsProperty(_controllerDataBuiltInput, "STRING", "builtInput");
        if(!value.isEmpty())
            properties.append(value);
    }


    if(properties.isEmpty())
        return 0;

    retObj->insert("properties", properties);
    return retObj;
}



//Paint with 3D effect
/*
void QWheel::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    AbstractWidget::PainterSetup(&painter);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QFontMetrics fm(this->font());

    // -- first calculate size and position.
    int w = width();
    int h = height();


    //Debug background
//    painter.setBrush(Qt::blue);
//    painter.drawRect(0,0,w,h);

    painter.setWindow(QRect(-w/2,0,w,h));

    int wheelWidth = w-w*0.2;

    QPalette palette = QApplication::palette();
    QPalette::ColorGroup colorGroup = isEnabled() ? QPalette::Active : QPalette::Disabled;

    QLinearGradient grad(0.5, 0, 0.5, 1.0);
    QColor gradColorDark = QColor(40,40,40);
    QColor gradColorLight = QColor(90,90,90);

    grad.setColorAt(0.0, gradColorDark);
    grad.setColorAt(0.2, gradColorLight);
    grad.setColorAt(0.8, gradColorLight);
    grad.setColorAt(1.0, gradColorDark);
    grad.setCoordinateMode( QGradient::ObjectBoundingMode );
    QBrush gBrush( grad );

    // paint a border and background
    painter.setPen(palette.color(colorGroup, QPalette::ButtonText));
    painter.setBrush(gBrush);
    // painter.setBrushOrigin( QPointF( 0.0, 0.0 ) );

    QPainterPath wheelPath = QPainterPath(QPoint(-w/2,0));
    wheelPath.addRoundedRect(QRect (-wheelWidth/ 2, 0, wheelWidth, h), 20.0, 90.0 );
    painter.drawPath(wheelPath);

    // paint the items
    painter.setClipPath(wheelPath);
    QColor penColor = QColor("black");
    painter.setPen(penColor);

   int iH = itemHeight();
   int iC = itemCount();
   if (iC > 0) {

       m_itemOffset = m_itemOffset % iH;

       int minItem = -h/2/iH;
       int maxItem = h/2/iH;

       for (int i=minItem; i<= maxItem; i++) {
            int itemNum = m_currentItem + i;
            if (itemNum < 0)
               continue;
            if (itemNum >= iC)
               continue;

            while (itemNum < 0)
               itemNum += iC;
            while (itemNum >= iC)
               itemNum -= iC;

            int itemYCenter = h/2 + i*iH - m_itemOffset;

            qreal itemYStretch = 1;
            qreal itemXStretch = 1;

            int radius = h / 2;
            int itemDistance = 0;

            itemDistance = qAbs(radius - itemYCenter);
            if (itemDistance > radius)
                itemDistance = radius;

            qreal itemRotation = qAsin((qreal)itemDistance / radius);
            qreal angularSize = 2 * qAsin((qreal)iH/2 / radius);
            qreal itemMinDistance = radius * qSin(itemRotation - angularSize / 2);
            qreal itemMaxDistance = radius * qSin(itemRotation + angularSize / 2);
            int rotatedSize = qAbs(itemMinDistance - itemMaxDistance);

            itemYStretch = (qreal) rotatedSize / iH ;
            itemXStretch = itemYStretch;


            painter.save();

            painter.translate(0, itemYCenter);
            painter.scale(itemXStretch + (1 - itemXStretch) / 2, itemYStretch);
            painter.drawLine(-w/2, +iH / 2, w, +iH / 2);
            if(itemNum == 0)
                painter.drawLine(-w/2, -iH / 2, w, -iH / 2);

            int maxTextWidth = wheelWidth - 10;
            int textWidth = fm.width(m_items.at(itemNum));
            if(textWidth > maxTextWidth)
                painter.scale((double)maxTextWidth / textWidth,1);


            painter.drawText(QRect(-textWidth/ 2, -iH / 2, textWidth, iH), Qt::AlignCenter, m_items.at(itemNum));

            painter.restore();
        }
    }

    // draw a transparent bar over the center
    QColor highlight = QColor("white");
    highlight.setAlpha(120);

    QLinearGradient grad2(0.5, 0, 0.5, 1.0);
    grad2.setColorAt(0, highlight);
    grad2.setColorAt(1.0, highlight.lighter());
    grad2.setCoordinateMode( QGradient::ObjectBoundingMode );
    QBrush gBrush2( grad2 );

    QLinearGradient grad3(0.5, 0, 0.5, 1.0);
    grad3.setColorAt(0, highlight);
    grad3.setColorAt(1.0, highlight.lighter());
    grad3.setCoordinateMode( QGradient::ObjectBoundingMode );
    QBrush gBrush3( grad3 );

    painter.fillRect( QRect(-w/2, h/2 - iH/2, w*1, iH/2 ), gBrush2 );
    painter.fillRect( QRect(-w/2, h/2,        w*1, iH/2 ), gBrush3 );
}
*/

//Paint with [0,0] in default position
/*
void QWheel::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    AbstractWidget::PainterSetup(&painter);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QFontMetrics fm(this->font());

    // -- first calculate size and position.
    int w = width();
    int h = height();

    //Debug background
//    painter.setBrush(Qt::blue);
//    painter.drawRect(0,0,w,h);

     //w=w-(w*0.2);

    int xMargin = w * 0.10;
    int wheelWidth = w-w*0.2;

    QPalette palette = QApplication::palette();
    QPalette::ColorGroup colorGroup = isEnabled() ? QPalette::Active : QPalette::Disabled;

    QLinearGradient grad(0.5, 0, 0.5, 1.0);

    QColor gradColorDark = QColor(40,40,40);
    QColor gradColorLight = QColor(90,90,90);

    grad.setColorAt(0.0, gradColorDark);
    grad.setColorAt(0.2, gradColorLight);
    grad.setColorAt(0.8, gradColorLight);
    grad.setColorAt(1.0, gradColorDark);
    grad.setCoordinateMode( QGradient::ObjectBoundingMode );
    QBrush gBrush( grad );

    // paint a border and background
    painter.setPen(palette.color(colorGroup, QPalette::ButtonText));
    painter.setBrush(gBrush);
    // painter.setBrushOrigin( QPointF( 0.0, 0.0 ) );

    QPainterPath wheelPath = QPainterPath(QPoint(0,0));
    wheelPath.addRoundedRect(QRect (w*0.1, 0, wheelWidth, h), 20.0, 90.0 );
    painter.drawPath(wheelPath);

    // paint the items
    painter.setClipPath(wheelPath);
    //painter.setClipRect( QRect( 30, 15, w-6, h-30 ) );

    QColor penColor = QColor("black");
    painter.setPen(penColor);

   int iH = itemHeight();
   int iC = itemCount();
   if (iC > 0) {

       m_itemOffset = m_itemOffset % iH;

       int minItem = -h/2/iH;
       int maxItem = h/2/iH;

       for (int i=minItem; i<= maxItem; i++) {
            int itemNum = m_currentItem + i;
            while (itemNum < 0)
               itemNum += iC;
            while (itemNum >= iC)
               itemNum -= iC;

            //int itemY = h/2 + i*iH - m_itemOffset - iH/2;

            int itemYCenter = h/2 + i*iH - m_itemOffset;

            qreal itemYStretch = 1;
            qreal itemXStretch = 1;

            if((i > 0) || true)
            {
                int radius = h / 2;
                int itemDistance = 0;

                itemDistance = qAbs(radius - itemYCenter); // - iH/2;
                if (itemDistance > radius)
                    itemDistance = radius;

                qreal itemRotation = qAsin((qreal)itemDistance / radius);
                qreal angularSize = 2 * qAsin((qreal)iH/2 / radius);
                qreal itemMinDistance = radius * qSin(itemRotation - angularSize / 2);
                qreal itemMaxDistance = radius * qSin(itemRotation + angularSize / 2);
                int rotatedSize = qAbs(itemMinDistance - itemMaxDistance);

                //itemStretch = (qreal) (h/2 - itemDistance) / (h/2);
                itemYStretch = (qreal) rotatedSize / iH ;
                itemXStretch = itemYStretch;

                //int itemXTranslate = (xMargin) * ((1 - itemXStretch) * 1.6);
                int itemXTranslate = (w * 0.15) * ((1 - itemXStretch) * 1.6);

//                int textWidth = fm.width(m_items.at(itemNum));
//                if(textWidth > wheelWidth)
//                    itemXStretch = 0.2; // (double)wheelWidth / textWidth;

                painter.save();

//                painter.translate(xMargin * (1 - itemStretch), itemYCenter);
//                painter.scale(itemStretch + (1 - itemStretch) / 2, itemStretch);
                painter.translate(itemXTranslate, itemYCenter);
                painter.scale(itemXStretch + (1 - itemXStretch) / 2, itemYStretch);
                //int yAdd = ((itemDistance) * itemStretch);
                //itemY += 24;


                painter.drawLine(0, +iH / 2, w, +iH / 2);
                //painter.drawLine(30, -iH / 2, 80, iH);

                painter.drawText(QRect(xMargin, -iH / 2, w - (xMargin * 2), iH), Qt::AlignCenter, m_items.at(itemNum));

                //Debug
                //painter.drawText(QRect(30, -iH / 2, 80, iH), Qt::AlignCenter, QString::number(itemDistance) + " " + QString::number(itemStretch,'f', 2));
                painter.restore();
            }

//                if((i <= 0 )&& false)
//                {
//                    //paintItem(&painter, itemNum, QRect(30, itemY, 80, iH));
//                    painter.drawText(QRect(30, itemY, 80, iH), Qt::AlignCenter, QString::number(itemY));
//                }
        }
    }

    // draw a transparent bar over the center
    QColor highlight = QColor("white");
    highlight.setAlpha(120);

    QLinearGradient grad2(0.5, 0, 0.5, 1.0);
    grad2.setColorAt(0, highlight);
    grad2.setColorAt(1.0, highlight.lighter());
    grad2.setCoordinateMode( QGradient::ObjectBoundingMode );
    QBrush gBrush2( grad2 );

    QLinearGradient grad3(0.5, 0, 0.5, 1.0);
    grad3.setColorAt(0, highlight);
    grad3.setColorAt(1.0, highlight.lighter());
    grad3.setCoordinateMode( QGradient::ObjectBoundingMode );
    QBrush gBrush3( grad3 );

    painter.fillRect( QRect( 0, h/2 - iH/2, w*1, iH/2 ), gBrush2 );
    painter.fillRect( QRect( 0, h/2,        w*1, iH/2 ), gBrush3 );
}
*/

//Paint with margin
/*
void QWheel::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);


    // -- first calculate size and position.
    int w = width();
    int h = height();

    //Debug background
    painter.setBrush(Qt::blue);
    painter.drawRect(0,0,w,h);

     w=w-30;

    int xMargin = w * 0.2;

    QPalette palette = QApplication::palette();
    QPalette::ColorGroup colorGroup = isEnabled() ? QPalette::Active : QPalette::Disabled;

    QLinearGradient grad(0.5, 0, 0.5, 1.0);
    grad.setColorAt(0.0, QColor(40,40,40));
    grad.setColorAt(0.2, QColor(90,90,90));
    grad.setColorAt(0.8, QColor(90,90,90));
    grad.setColorAt(1.0, QColor(40,40,40));
    grad.setCoordinateMode( QGradient::ObjectBoundingMode );
    QBrush gBrush( grad );

    // paint a border and background
    painter.setPen(palette.color(colorGroup, QPalette::ButtonText));
    painter.setBrush(gBrush);
    // painter.setBrushOrigin( QPointF( 0.0, 0.0 ) );

    QPainterPath wheelPath = QPainterPath(QPoint(w*0.2, h*0.08));
    wheelPath.addRoundedRect(QRect (w*0.2, h*0.09, w * 0.533, h-(h*0.186)), 20.0, 70.0 );
    painter.drawPath(wheelPath);

    // paint the items
    painter.setClipPath(wheelPath);
    //painter.setClipRect( QRect( 30, 15, w-6, h-30 ) );
    painter.setPen(QColor("black"));

   int iH = itemHeight();
   int iC = itemCount();
   if (iC > 0) {

       m_itemOffset = m_itemOffset % iH;

       int minItem = -h/2/iH;
       int maxItem = h/2/iH;

       for (int i=minItem; i<= maxItem; i++) {
            int itemNum = m_currentItem + i;
            while (itemNum < 0)
               itemNum += iC;
            while (itemNum >= iC)
               itemNum -= iC;

            //int itemY = h/2 + i*iH - m_itemOffset - iH/2;

            int itemYCenter = h/2 + i*iH - m_itemOffset;

            qreal itemStretch = 1;

            if((i > 0) || true)
            {
                int radius = h / 2;
                int itemDistance = 0;

                itemDistance = qAbs(radius - itemYCenter); // - iH/2;
                if (itemDistance > radius)
                    itemDistance = radius;

                qreal itemRotation = qAsin((qreal)itemDistance / radius);
                qreal angularSize = 2 * qAsin((qreal)iH/2 / radius);
                qreal itemMinDistance = radius * qSin(itemRotation - angularSize / 2);
                qreal itemMaxDistance = radius * qSin(itemRotation + angularSize / 2);
                int rotatedSize = qAbs(itemMinDistance - itemMaxDistance);

                //itemStretch = (qreal) (h/2 - itemDistance) / (h/2);
                itemStretch = (qreal) rotatedSize / iH ;

                painter.save();

                painter.translate(xMargin * (1 - itemStretch), itemYCenter);
                painter.scale(itemStretch + (1 - itemStretch) / 2, itemStretch);
                //int yAdd = ((itemDistance) * itemStretch);
                //itemY += 24;

                painter.drawLine(xMargin, +iH / 2, w * 0.933, +iH / 2);
                //painter.drawLine(30, -iH / 2, 80, iH);
                painter.drawText(QRect(xMargin, -iH / 2, w*0.533, iH), Qt::AlignCenter, m_items.at(itemNum));

                //Debug
                //painter.drawText(QRect(30, -iH / 2, 80, iH), Qt::AlignCenter, QString::number(itemDistance) + " " + QString::number(itemStretch,'f', 2));
                painter.restore();
            }

//                if((i <= 0 )&& false)
//                {
//                    //paintItem(&painter, itemNum, QRect(30, itemY, 80, iH));
//                    painter.drawText(QRect(30, itemY, 80, iH), Qt::AlignCenter, QString::number(itemY));
//                }
        }
    }

    // draw a transparent bar over the center
    QColor highlight = QColor("white");
    highlight.setAlpha(120);

    QLinearGradient grad2(0.5, 0, 0.5, 1.0);
    grad2.setColorAt(0, highlight);
    grad2.setColorAt(1.0, highlight.lighter());
    grad2.setCoordinateMode( QGradient::ObjectBoundingMode );
    QBrush gBrush2( grad2 );

    QLinearGradient grad3(0.5, 0, 0.5, 1.0);
    grad3.setColorAt(0, highlight);
    grad3.setColorAt(1.0, highlight.lighter());
    grad3.setCoordinateMode( QGradient::ObjectBoundingMode );
    QBrush gBrush3( grad3 );

    painter.fillRect( QRect( xMargin, h/2 - iH/2, w*0.533, iH/2 ), gBrush2 );
    painter.fillRect( QRect( xMargin, h/2,        w*0.533, iH/2 ), gBrush3 );
}
*/

//Paint using viewport
/*
void QWheel::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int side = qMin(width(), height());
    painter.setViewport((width() - side) / 2, (height() - side) / 2,side, side);
    painter.setWindow(0,0, 150, 150);

    // -- first calculate size and position.
    int w = 150;
    int h = 150;

     w=w-30;

    QPalette palette = QApplication::palette();
    QPalette::ColorGroup colorGroup = isEnabled() ? QPalette::Active : QPalette::Disabled;



    QLinearGradient grad(0.5, 0, 0.5, 1.0);
    grad.setColorAt(0.0, QColor(40,40,40));
    grad.setColorAt(0.2, QColor(90,90,90));
    grad.setColorAt(0.8, QColor(90,90,90));
    grad.setColorAt(1.0, QColor(40,40,40));
    grad.setCoordinateMode( QGradient::ObjectBoundingMode );
    QBrush gBrush( grad );

    // paint a border and background
    painter.setPen(palette.color(colorGroup, QPalette::ButtonText));
    painter.setBrush(gBrush);
    // painter.setBrushOrigin( QPointF( 0.0, 0.0 ) );

    QPainterPath wheelPath = QPainterPath(QPoint(30, 12));
    wheelPath.addRoundedRect(QRect (30, 14, 80, h-28 ), 20.0, 70.0 );
    painter.drawPath(wheelPath);

    // paint the items
    painter.setClipPath(wheelPath);
    //painter.setClipRect( QRect( 30, 15, w-6, h-30 ) );
    painter.setPen(QColor("black"));

   int iH = itemHeight();
   int iC = itemCount();
   if (iC > 0) {

       m_itemOffset = m_itemOffset % iH;

       int minItem = -h/2/iH;
       int maxItem = h/2/iH;

       for (int i=minItem; i<= maxItem; i++) {
            int itemNum = m_currentItem + i;
            while (itemNum < 0)
               itemNum += iC;
            while (itemNum >= iC)
               itemNum -= iC;

            //int itemY = h/2 + i*iH - m_itemOffset - iH/2;

            int itemYCenter = h/2 + i*iH - m_itemOffset;

            qreal itemStretch = 1;

            if((i > 0) || true)
            {
                int radius = h / 2;
                int itemDistance = 0;

                itemDistance = qAbs(radius - itemYCenter); // - iH/2;
                if (itemDistance > radius)
                    itemDistance = radius;

                qreal itemRotation = qAsin((qreal)itemDistance / radius);
                qreal angularSize = 2 * qAsin((qreal)iH/2 / radius);
                qreal itemMinDistance = radius * qSin(itemRotation - angularSize / 2);
                qreal itemMaxDistance = radius * qSin(itemRotation + angularSize / 2);
                int rotatedSize = qAbs(itemMinDistance - itemMaxDistance);

                //itemStretch = (qreal) (h/2 - itemDistance) / (h/2);
                itemStretch = (qreal) rotatedSize / iH ;


                painter.save();

                painter.translate(30 * (1 - itemStretch), itemYCenter);
                painter.scale(itemStretch + (1 - itemStretch) / 2, itemStretch);
                //int yAdd = ((itemDistance) * itemStretch);
                //itemY += 24;

                painter.drawLine(30, +iH / 2, 140, +iH / 2);
                //painter.drawLine(30, -iH / 2, 80, iH);
                painter.drawText(QRect(30, -iH / 2, 80, iH), Qt::AlignCenter, m_items.at(itemNum));

                //Debug
                //painter.drawText(QRect(30, -iH / 2, 80, iH), Qt::AlignCenter, QString::number(itemDistance) + " " + QString::number(itemStretch,'f', 2));
                painter.restore();
            }

//                if((i <= 0 )&& false)
//                {
//                    //paintItem(&painter, itemNum, QRect(30, itemY, 80, iH));
//                    painter.drawText(QRect(30, itemY, 80, iH), Qt::AlignCenter, QString::number(itemY));
//                }
        }
    }

    // draw a transparent bar over the center
    QColor highlight = QColor("white");
    highlight.setAlpha(120);

    QLinearGradient grad2(0.5, 0, 0.5, 1.0);
    grad2.setColorAt(0, highlight);
    grad2.setColorAt(1.0, highlight.lighter());
    grad2.setCoordinateMode( QGradient::ObjectBoundingMode );
    QBrush gBrush2( grad2 );

    QLinearGradient grad3(0.5, 0, 0.5, 1.0);
    grad3.setColorAt(0, highlight);
    grad3.setColorAt(1.0, highlight.lighter());
    grad3.setCoordinateMode( QGradient::ObjectBoundingMode );
    QBrush gBrush3( grad3 );

    painter.fillRect( QRect( 30, h/2 - iH/2, 80, iH/2 ), gBrush2 );
    painter.fillRect( QRect( 30, h/2,        80, iH/2 ), gBrush3 );

}

*/

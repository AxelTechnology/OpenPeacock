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

#include "abstractwidget.h"
#include "onairclockwidget.h"
#include "logger.h"


OnairclockWidget::OnairclockWidget(QWidget *parent)
    : AbstractWidget(parent)
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateClock()));
    timer->start(100);

    _parent = parent;
    //setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setWindowFlags(Qt::Widget); // | Qt::FramelessWindowHint);

    //setStyleSheet("background-color:none;");
//    QPalette pal = this->palette();
//    pal.setColor(QPalette::Base, Qt::cyan); // QColor(200,200,200);
//    this->setPalette(pal);

//    setAttribute(Qt::WA_NoSystemBackground);
//    setAttribute(Qt::WA_TranslucentBackground);

    //setAttribute(Qt::WA_PaintOnScreen);
    //setAttribute(Qt::WA_TransparentForMouseEvents);

    //setWindowTitle(tr("On Air Clock"));
    //_image = QImage(":/AxelTechnology.jpg");
    _image = QImage(":/AxelLogoWhite.png");

    _colorFiveSec = QColor(Qt::yellow);
    _colorSec = QColor(Qt::white);
    _colorFont = QColor(Qt::white);

    //resize(800, 480);

}

OnairclockWidget::~OnairclockWidget()
{
    if(timer != 0)
    {
        timer->stop();
        delete timer;
    }
}

void OnairclockWidget::updateClock()
{
    int currentTotalSec = QTime::currentTime().msecsSinceStartOfDay() / 1000;
    int drawnTotalSec = _time.msecsSinceStartOfDay() / 1000;
    if (currentTotalSec == drawnTotalSec)
        return;
    Logger::Instance()->AddLog("OnairClockWidget: current second=" + QString::number(currentTotalSec) + " old value=" + QString::number(drawnTotalSec), LoggerLevel_DEBUGVERBOSE, "CLOCK_UPD_" + this->objectName());
    update();
}


void OnairclockWidget::resizeEvent(QResizeEvent *event)
{
    AbstractWidget::resizeEvent(event);
    ResizeClock(new QRect(QPoint(0,0), event->size()));
    Q_UNUSED(event);
    update();

}

void OnairclockWidget::paintEvent(QPaintEvent * event)
{

    Q_UNUSED(event)

    _time = QTime::currentTime();
    //QPainter* painter = new QPainter (this);

    QPainter painter(this);
    AbstractWidget::PainterSetup(&painter);

    //Background
    //painter.fillRect(0, 0, width(), height(), Qt::black);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);

//    painter->setRenderHint(QPainter::Antialiasing, true);
//    painter->setPen(Qt::NoPen);


    //DrawFiveSeconds(& painter);
//    QRect rect;
//    painter.setBrush(QColor(Qt::yellow));
//    //double dStep = qDegreesToRadians(30);

//    for (int i = 0; i <= 360; i += 30) {
//        rect = QRect(int(_centerX + _radiusFiveSeconds * qCos(qDegreesToRadians((float)i)) - RADIUS_FIVE_SECONDS),
//                             int(_centerY + _radiusFiveSeconds * qSin(qDegreesToRadians((float)i)) - RADIUS_FIVE_SECONDS),
//                             int(2 * RADIUS_FIVE_SECONDS),
//                             int(RADIUS_FIVE_SECONDS * 2));

//        painter.drawEllipse(rect);
//    }



    DrawFiveSeconds(&painter);
    DrawTime(& painter);
    DrawTextTime(& painter);
    DrawImage(& painter);
//    DrawFiveSeconds(painter);
//    DrawTime(painter);
//    DrawTextTime(painter);
//    DrawImage(painter);

}


void OnairclockWidget::DrawFiveSeconds(QPainter* p)
{
    QRect rect;
    p->setBrush(_colorFiveSec);
    //double dStep = qDegreesToRadians(30);

    for (int i = 0; i <= 360; i += 30) {
        rect = QRect(int(_centerX + _radiusFiveSeconds * qCos(qDegreesToRadians((float)i)) - RADIUS_FIVE_SECONDS),
                             int(_centerY + _radiusFiveSeconds * qSin(qDegreesToRadians((float)i)) - RADIUS_FIVE_SECONDS),
                             int(2 * RADIUS_FIVE_SECONDS),
                             int(RADIUS_FIVE_SECONDS * 2));

        p->drawEllipse(rect);
    }
}

void OnairclockWidget::DrawTime(QPainter* p)
{
    p->setBrush(_colorSec);

    int iSecond = _time.second();
    QRect rect;
    double i = 0;
    for (int c = 0; c <= iSecond * 6; c += 6) {
        i = qDegreesToRadians((float)c - 90);
        rect = QRect(int(_centerX + _radiusSeconds * qCos(i) - RADIUS_SECONDS * 3 / 4),
                     int(_centerY + _radiusSeconds * qSin(i) - RADIUS_SECONDS * 3 / 4),
                     int(RADIUS_SECONDS * 3 / 2),
                     int(RADIUS_SECONDS * 3 / 2));

        p->drawEllipse(rect);
    }
}

void OnairclockWidget::DrawTextTime(QPainter* p)
{
    QString sText;

    p->setFont(_fontTextTime);
    p->setPen(QPen(_colorFont));

    /* DEBUG rectangles
    p->drawRect(_rectDigit1);
    p->drawRect(_rectDigit2);
    p->drawRect(_rectDigitDP);
    p->drawRect(_rectDigit3);
    p->drawRect(_rectDigit4);
    p->drawRect(_rectDigitSec1);
    p->drawRect(_rectDigitSec2);
    */


    if(_time.second() % 2 == 0)
        sText = ":";
    else
        sText = " ";
    p->drawText(_rectDigitDP, Qt::AlignVCenter, sText);

    //Hour
    sText = QString("%1").arg(_time.hour(), 2, 10, QChar('0'));
    QString cToWrite = sText.mid(0, 1);
    p->drawText(_rectDigit1, Qt::AlignRight | Qt::AlignVCenter, cToWrite);
    cToWrite = sText.mid(1, 1);
    p->drawText(_rectDigit2, Qt::AlignRight | Qt::AlignVCenter, cToWrite);
    //Minute
    sText = QString("%1").arg(_time.minute(), 2, 10, QChar('0'));
    cToWrite = sText.mid(0, 1);
    p->drawText(_rectDigit3, Qt::AlignRight | Qt::AlignVCenter, cToWrite);
    cToWrite = sText.mid(1, 1);
    p->drawText(_rectDigit4, Qt::AlignRight | Qt::AlignVCenter, cToWrite);


    p->setFont(_fontTextSeconds);

    //Seconds
//    sText = string.Format("{0:d2}", _TimeSpan.Seconds);
//    cToWrite = Convert.ToChar(sText.Substring(0, 1));
//    g.DrawString(cToWrite, _fontTextSeconds, b, _rectDigitSec1, sf);
//    cToWrite = Convert.ToChar(sText.Substring(1, 1));
//    g.DrawString(cToWrite, _fontTextSeconds, b, _rectDigitSec2, sf);
    //sText = QString("%1").arg(_time.second(), 2, 10, QChar('0'));
    sText = QString("%1").arg(_time.second(), 2, 10, QChar('0'));
    cToWrite = sText.mid(0, 1);
    p->drawText(_rectDigitSec1, Qt::AlignRight | Qt::AlignVCenter, cToWrite);
    cToWrite = sText.mid(1, 1);
    p->drawText(_rectDigitSec2, Qt::AlignRight | Qt::AlignVCenter, cToWrite);
}

void OnairclockWidget::DrawImage(QPainter* p)
{
    QRect r((_centerX - _imageSize.width() / 2),
            (_rectImage.y() + _rectImage.height() / 2 - _imageSize.height() / 2),
            _imageSize.width(),
            _imageSize.height());

//        p->setBrush(Qt::white);
//        p->drawRect(_rectImage);

    p->drawImage(QPoint(r.x(), r.y()), _image);


}


void OnairclockWidget::ResizeClock(QRect *drawingArea)
{

    _centerX = drawingArea->x() + int(drawingArea->width() / 2);
    _centerY = drawingArea->y() + int(drawingArea->height() / 2);

    //Step di separazione tra i valori dell'orario...
    STEP_SEPARATOR = _centerX / 150;

    if (_centerX < _centerY) {
        RADIUS_FIVE_SECONDS = _centerX / 30;
        _radiusFiveSeconds = _centerX - drawingArea->x() - RADIUS_FIVE_SECONDS * 1.5;
    } else {
        RADIUS_FIVE_SECONDS = _centerY / 34;
        _radiusFiveSeconds = _centerY - drawingArea->y() - RADIUS_FIVE_SECONDS * 1.5;
    }
    RADIUS_SECONDS = RADIUS_FIVE_SECONDS * 0.9;
    _radiusSeconds = _radiusFiveSeconds - RADIUS_FIVE_SECONDS * 2;

    //calculate rectangular size of time and seconds text
    int dStartX = 0;
    int dStartY = 0;
    QSize _size;
    QSize _sizeDP;

    _TextTimeHeight = (_radiusFiveSeconds * 3 / 5);
    _TextSecondsHeight = (_radiusFiveSeconds * 2 / 5);


    //Logger::Instance()->AddLog("OnairclockWidget: font calculate 1", LoggerLevel_DEBUGVERBOSE);

    //Calcolo il Font per l'orario...
    FontCalculate();

    //Logger::Instance()->AddLog("OnairclockWidget: font calculate 2", LoggerLevel_DEBUGVERBOSE);

    //QFontMetrics fmTime(_fontTextTime);

    int textWidth = _TextTimeHeight * 0.299; // fmTime.width(":");

    _sizeDP = QSize(textWidth, _fontTextSeconds.pointSize() * 1.3); //objGraphics.MeasureString(":", _fontTextTime);
    dStartY = int(_centerY - (_sizeDP.height() * 0.5));

    //Logger::Instance()->AddLog("OnairclockWidget: font calculate 3", LoggerLevel_DEBUGVERBOSE);

    //XX:XX
    dStartX = int(_centerX - (_sizeDP.width() / 2));
    _rectDigitDP = QRect(dStartX, dStartY, int(_sizeDP.width()), int(_sizeDP.height()));


    textWidth = _TextTimeHeight * 0.6775;  // fmTime.width("0");

    _size = QSize(textWidth, _fontTextSeconds.pointSize() * 1.3); //objGraphics.MeasureString("0", _fontTextTime);
    dStartY = int(_centerY - (_size.height() * 0.5));

    //Logger::Instance()->AddLog("OnairclockWidget: font calculate 4", LoggerLevel_DEBUGVERBOSE);

    //X0XXX
    dStartX = int(_centerX - _size.width() - (_sizeDP.width() / 4));
    _rectDigit2 = QRect(dStartX, dStartY, int(_size.width()), int(_size.height()));
    //XXX0X
    dStartX = int(_centerX + (_sizeDP.width() / 4));
    _rectDigit3 = QRect(dStartX, dStartY, int(_size.width()), int(_size.height()));

    //0XXXX
    //dStartX = int(_centerX - (_size.width() * 2) + (_sizeDP.width() / 8 * 3));
    //_rectDigit1 = QRect(int(dStartX - STEP_SEPARATOR), dStartY, int(_size.width()), int(_size.height()));
    dStartX = int(_centerX - (_size.width() * 2) - STEP_SEPARATOR);
    _rectDigit1 = QRect(dStartX, dStartY, int(_size.width()), int(_size.height()));

    //XXXX0
    //dStartX = int(_centerX + (_size.width() * 2 / 3)); // + (_sizeDP.width() / 8 * 3));
    //_rectDigit4 = QRect(int(dStartX + STEP_SEPARATOR), dStartY, int(_size.width()), int(_size.height()));
    dStartX = int(_centerX + (_size.width()) + STEP_SEPARATOR); // + (_sizeDP.width() / 8 * 3));
    _rectDigit4 = QRect(dStartX, dStartY, int(_size.width()), int(_size.height()));


//    float sBufX = _radiusSeconds * qSin(qDegreesToRadians(30.0)) - RADIUS_FIVE_SECONDS;
//    float sBufY = _radiusSeconds * qCos(qDegreesToRadians(30.0)) - RADIUS_FIVE_SECONDS;

    float bufWidth = (_radiusSeconds * qCos(qDegreesToRadians(60.0)) - RADIUS_FIVE_SECONDS) * 2;
    float dImageStartX = _centerX - (bufWidth / 2); // _radiusSeconds * qCos(qDegreesToRadians(70.0)) + RADIUS_FIVE_SECONDS * 2;
    //dStartY = _centerY - sBufY + RADIUS_FIVE_SECONDS + (STEP_SEPARATOR * 2);
    float dImageStartY = _centerY - _radiusSeconds * qSin(qDegreesToRadians(60.0)) + RADIUS_FIVE_SECONDS * 2;
    float bufHeight = (_centerY - dImageStartY) - (_centerY - dStartY);



//    _rectImage = QRect(int(dStartX),
//                       int(dStartY),
//                       int( (_radiusSeconds * qSin(qDegreesToRadians(30.0)) * 2)),
//                       int(sBufX));
    _rectImage = QRect(int(dImageStartX),
                       int(dImageStartY),
                       int(bufWidth),
                       int(bufHeight));

    ResizeImage();


    //QFontMetrics fmSeconds(_fontTextSeconds);
    textWidth = _TextSecondsHeight * 0.6769; // fmSeconds.width("0");

    _size = QSize(textWidth, _TextSecondsHeight);
    dStartY = int(_centerY + (_size.height() * 0.75));

    //0X
    //dStartX = int(_centerX - (_size.width() * 1 ));
    dStartX = int(_centerX - (_size.width())); // - (STEP_SEPARATOR / 2));
    _rectDigitSec1 = QRect(dStartX, dStartY, int(_size.width()), int(_size.height()));
    //X0
    //dStartX = int(_centerX + STEP_SEPARATOR);
    dStartX = int(_centerX + (STEP_SEPARATOR / 2));
    _rectDigitSec2 = QRect(dStartX, dStartY, int(_size.width()), int(_size.height()));

}




void OnairclockWidget::ResizeImage()
{
    if (_image.isNull())
        return;

    if (_image.height() > _image.width()) {
        if ((_image.width() / _image.height()) > (_rectImage.width() / _rectImage.height())) {
            _imageSize = QSize(_rectImage.width(), int(_rectImage.width() * _image.height() / _image.width()));
        } else {
            _imageSize = QSize(int(_rectImage.height() * _image.width() / _image.height()), _rectImage.height());
        }
    } else {
        if ((_image.width() / _image.height()) > (_rectImage.width() / _rectImage.height())) {
            _imageSize = QSize(_rectImage.width(), int(_rectImage.width() * _image.height() / _image.width()));
        } else {
            _imageSize = QSize(int(_rectImage.height() * _image.width() / _image.height()), _rectImage.height());
        }
    }

    QImage * imgBuff = new QImage(":/AxelLogoWhite.png");
    _image = imgBuff->scaled(_imageSize.width(), _imageSize.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    delete imgBuff;
    imgBuff = NULL;
}




void OnairclockWidget::FontCalculate()
{
//    int id = QFontDatabase::addApplicationFont(":/DS-DIGIB.TTF");
//    if (QFontDatabase::applicationFontFamilies(id).count() <= 0)
//            return;
//    QString fontFamily = QFontDatabase::applicationFontFamilies(id).at(0);

    QString fontFamily = "DS-Digital";

    if (!(_TextTimeHeight <= 0)) {
        _fontTextTime = QFont(fontFamily, _TextTimeHeight);
    } else {
        _fontTextTime = QFont(fontFamily, 10);
    }
    if (!(_TextSecondsHeight <= 0)) {
        _fontTextSeconds = QFont(fontFamily, _TextSecondsHeight);
    } else {
        _fontTextSeconds = QFont(fontFamily, 10);
    }
}



bool OnairclockWidget::ManageRemoteCommand(QString propertyName, QJsonValue value, bool init, QString * error)
{
    Q_UNUSED(error);
    bool retVal = AbstractWidget::ManageRemoteCommand(propertyName, value, init, error);

    if(!retVal)
        error = new QString("Error setting OnairclockWidget timezone " + this->objectName() + " property=" + propertyName + " value=" + value.toString() );

    return retVal;
}

void OnairclockWidget::SetJsonData(QJsonObject *data)
{
    if (data == 0)
        return;

    _colorSec = GetColorFromString((*data)["colorSec"].toString());
    _colorFiveSec = GetColorFromString((*data)["colorFiveSec"].toString());
    _colorFont = GetColorFromString((*data)["colorFont"].toString());

    AbstractWidget::SetJsonData(data);
}


QJsonObject* OnairclockWidget::GetJsonData()
{
    QJsonObject* retObj = AbstractWidget::GetJsonData();

    retObj->insert("colorSec", QJsonValue(GetStringFromColor(_colorSec)));
    retObj->insert("colorFiveSec", QJsonValue(GetStringFromColor(_colorFiveSec)));
    retObj->insert("colorFont", QJsonValue(GetStringFromColor(_colorFont)));

    return retObj;
}

QJsonObject *OnairclockWidget::GetJsonDataVars()
{
    QJsonObject* retObj = AbstractWidget::GetJsonDataVars();

    if((retObj == 0) || (!retObj->contains("properties")))
        return 0;

    return retObj;
}

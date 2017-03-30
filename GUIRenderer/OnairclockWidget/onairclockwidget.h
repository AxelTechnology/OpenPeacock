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

#ifndef ANALOGCLOCK_H
#define ANALOGCLOCK_H

#include "abstractwidget.h"

#include <QWidget>
#include <QtWidgets>

class OnairclockWidget : public AbstractWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor colorFiveSec READ colorFiveSec WRITE setColorFiveSec)
    Q_PROPERTY(QColor colorSec READ colorSec WRITE setColorSec)
    Q_PROPERTY(QColor colorFont READ colorFont WRITE setColorFont)

public:
    virtual ~OnairclockWidget();

    explicit OnairclockWidget(QWidget *parent = 0);
    bool ManageRemoteCommand(QString propertyName, QJsonValue value, bool init, QString * error) Q_DECL_OVERRIDE;
    void SetJsonData(QJsonObject* data) Q_DECL_OVERRIDE;
    QJsonObject* GetJsonData() Q_DECL_OVERRIDE;
    QJsonObject* GetJsonDataVars() Q_DECL_OVERRIDE;

    QColor colorFiveSec(){ return _colorFiveSec;}
    void setColorFiveSec(QColor value){ _colorFiveSec = value; update();}
    QColor colorSec(){ return _colorSec;}
    void setColorSec(QColor value){ _colorSec = value; update();}
    QColor colorFont(){ return _colorFont;}
    void setColorFont(QColor value){ _colorFont = value; update();}


protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
    void ResizeClock(QRect *drawingArea);
    void ResizeImage();
    void FontCalculate();
    void DrawFiveSeconds(QPainter* p);
    void DrawTime(QPainter* p);
    void DrawTextTime(QPainter* p);
    void DrawImage(QPainter* p);

private slots:
    void updateClock();

private:
    double STEP_SEPARATOR;
    double RADIUS_FIVE_SECONDS;
    double RADIUS_SECONDS;
    double _radiusFiveSeconds;
    double _radiusSeconds;
    float _centerX;

    float _centerY;
    float _TextTimeHeight;
    float _TextSecondsHeight;
    QRect _rectDigit1;
    QRect _rectDigit2;
    QRect _rectDigitDP;
    QRect _rectDigit3;

    QRect _rectDigit4;
    QRect _rectDigitSec1;

    QRect _rectDigitSec2;
    QFont _fontTextTime;

    QColor _colorFiveSec;
    QColor _colorSec;
    QColor _colorFont;

    QTimer *timer;
    QFont _fontTextSeconds;
    QRect _rectImage;
    QImage _image;
    QSize _imageSize;
    QTime _time;
    QWidget *_parent;
};

#endif

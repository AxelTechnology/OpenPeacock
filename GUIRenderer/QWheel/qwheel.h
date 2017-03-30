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

#ifndef WHEELWIDGET_H
#define WHEELWIDGET_H

#include <QWidget>
#include <QStringList>
#include <QList>
#include <wheeldata.h>
#include <wheelconfiguration.h>
#include <abstractwidget.h>


QT_BEGIN_NAMESPACE
class QPainter;
class QRect;
QT_END_NAMESPACE

class QWheel : public AbstractWidget {
    Q_OBJECT
    Q_ENUMS(QWheelMode)

    Q_PROPERTY(int currentIndex READ currentIndex  WRITE setValue)
    Q_PROPERTY(int instantIndex READ instantIndex  WRITE setInstantValue)
    Q_PROPERTY(QStringList items READ items  WRITE setItems)
    Q_PROPERTY(QString builtInput READ builtInput WRITE setBuiltInput)

    Q_PROPERTY(QColor selectedColor READ selectedColor WRITE setSelectedColor)
    Q_PROPERTY(QColor unselectedColor READ unselectedColor WRITE setUnselectedColor)
    Q_PROPERTY(QColor backgroundWheel READ backgroundWheel WRITE setBackgroundWheel)

    Q_PROPERTY(int wheelModeInt READ wheelModeInt WRITE setWheelModeInt)
    Q_PROPERTY(QWheelMode wheelMode READ wheelMode WRITE setWheelMode)

    Q_PROPERTY(QString controllerDataValue READ controllerDataValue WRITE setControllerDataValue)
    Q_PROPERTY(QString controllerDataValues READ controllerDataValues WRITE setControllerDataValues)
    Q_PROPERTY(QString controllerDataInstantValue READ controllerDataInstantValue WRITE setControllerDataInstantValue)
    Q_PROPERTY(QString controllerDataSelectedValues READ controllerDataSelectedValues WRITE setControllerDataSelectedValues)
    Q_PROPERTY(QString controllerDataBuiltInput READ controllerDataBuiltInput WRITE setControllerDataBuiltInput)


public:

    enum QWheelMode
    {
        QWheelMode_SINGLESELECTION = 0,        
        QWheelMode_MULTISELECTION = 1,
        QWheelMode_STRINGBUILD = 2,
        QWheelMode_SINGLESELCTION_INSTANT_ONLY = 3
    };

    QWheel( QWidget *parent = 0,WheelConfiguration *configuration=NULL);
    QWheel( long id);


    virtual ~QWheel();

    int currentIndex() { return m_currentItem; }
    void setCurrentIndex(int index)
    {
        if (index >= 0 && index < itemCount()) {

            //m_itemOffset = 0;
            //update();
            setValue(index);
        }
    }

    int instantIndex() const { return m_instantValue; }
    void setInstantValue(int instantIndex);

//    QStringList currentItems() const;
//    void setCurrentItems(QStringList items);

    bool event(QEvent*);
    void paintEvent(QPaintEvent *e);
    void paintItem(QPainter* painter, int index, const QRect &rect) ;
    QStringList items() const;
    void setItems( const QStringList &items );

    QString builtInput() const { return m_builtInput; }
    void setBuiltInput(QString builtInput) { m_builtInput = builtInput; update();}

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    int itemHeight() const;
    int itemCount() const;

    void updateData(WheelData &data);

    void configure(WheelConfiguration &configuration);

    bool ManageRemoteCommand(QString propertyName, QJsonValue value, bool init, QString * error) Q_DECL_OVERRIDE;
    void SetJsonData(QJsonObject* data) Q_DECL_OVERRIDE;
    QJsonObject* GetJsonData() Q_DECL_OVERRIDE;
    QJsonObject* GetJsonDataVars() Q_DECL_OVERRIDE;

    QString controllerDataValue() const { return _controllerDataValue; }
    void setControllerDataValue(QString controllerDataValueId) { _controllerDataValue = controllerDataValueId; }

    QColor selectedColor() const { return _selectedColor; }
    void setSelectedColor(QColor foreground)
    {
        _selectedColor = foreground;
        _selectedColorShadowed = QColor(_selectedColor.red()*0.3, _selectedColor.green()*0.3, _selectedColor.blue()*0.3, _selectedColor.alpha());
        update();
    }

    QColor unselectedColor() const { return _unselectedColor; }
    void setUnselectedColor(QColor editingColor)
    {
        _unselectedColor = editingColor;
        _unselectedColorShadowed = QColor(_unselectedColor.red()*0.20, _unselectedColor.green()*0.20, _unselectedColor.blue()*0.20, _unselectedColor.alpha());
        update();
    }

    QColor backgroundWheel() const { return m_backgroundWheel; }
    void setBackgroundWheel(QColor backgroundWheel) { m_backgroundWheel = backgroundWheel; update();}


    QWheelMode wheelMode() const { return m_wheelMode; }
    void setWheelMode(QWheelMode mode) { m_wheelMode = mode; update();}
    int wheelModeInt() const { return (int) m_wheelMode; }
    void setWheelModeInt(int mode) { m_wheelMode = (QWheelMode) mode; update(); }

    QString controllerDataValues() const { return _controllerDataValues; }
    void setControllerDataValues(QString controllerDataValues) { _controllerDataValues = controllerDataValues; }

    QString controllerDataInstantValue() const { return _controllerDataInstantValue; }
    void setControllerDataInstantValue(QString controllerDataInstantValue) { _controllerDataInstantValue = controllerDataInstantValue; }

    QString controllerDataSelectedValues() const { return _controllerDataSelectedValues; }
    void setControllerDataSelectedValues(QString selectedValues) { _controllerDataSelectedValues = selectedValues; }

    QString controllerDataBuiltInput() const { return _controllerDataBuiltInput; }
    void setControllerDataBuiltInput(QString controllerDataInputString) { _controllerDataBuiltInput = controllerDataInputString; }


public slots:

    void scrollTo();
    void setValue(int);





signals:
    void stopped(int index);



protected:
    int m_currentItem;
    int m_instantValue;
    int m_lastReceivedValue;
    int m_lastReceivedInstantValue;
    bool m_editingValue;
    int _newItem;
    int m_itemOffset; // 0-itemHeight()
    qreal m_lastY;
private:

    void fireScrollEvent(bool);
    bool _initializing;
    bool _scrolling;
    bool SetJsonValues(QJsonValue* values);
    bool SetJsonSelectedValues(QJsonValue* values);
    void SetFirstSelectedValue();
    QStringList m_items;
    QList<int> m_selectedItems;


    QTimer* _tmrSetvalue;
    QString _controllerDataValue;
    QString _controllerDataValues;
    QString _controllerDataInstantValue;
    QString _controllerDataSelectedValues;

    QColor _selectedColor;
    QColor _unselectedColor;
    QColor _selectedColorShadowed;
    QColor _unselectedColorShadowed;
    QColor m_backgroundWheel;
    QWheelMode m_wheelMode;

    QString m_builtInput;
    QString _controllerDataBuiltInput;
};



#endif // WHEELWIDGET_H

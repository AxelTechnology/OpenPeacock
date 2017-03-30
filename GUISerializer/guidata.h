#ifndef MIXERGUIDATA_H
#define MIXERGUIDATA_H

#include "abstractwidget.h"
#include <QColor>
#include <QObject>
#include <QSize>

class GUIData
{
    Q_PROPERTY(QSize totalSize READ totalSize WRITE setTotalSize)
    Q_PROPERTY(QList<QWidget*>* widgetList READ widgetList WRITE setWidgetList)

public:
    GUIData();

    QSize totalSize(){ return _totalSize; }
    void setTotalSize(QSize value){ _totalSize = value; }

    QList<QWidget*>* widgetList(){ return _widgetList; }
    void setWidgetList(QList<QWidget*>* value){ _widgetList = value; }

private:
    QSize _totalSize;
    QList<QWidget*>* _widgetList;


};

#endif // MIXERGUIDATA_H

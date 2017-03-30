#include "guidata.h"

GUIData::GUIData()
{
    _widgetList = new QList<QWidget*>();
    _totalSize = QSize(800, 480);
}

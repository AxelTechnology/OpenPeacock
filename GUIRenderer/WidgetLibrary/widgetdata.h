#ifndef WIDGETDATA_H
#define WIDGETDATA_H

#include "widgetposition.h"
#include "widgetconfiguration.h"

class WidgetData
{
public:
    WidgetData();
    WidgetData(long id);

    long id;
    WidgetPosition * position;
    WidgetConfiguration * configuration;


};

#endif // WIDGETDATA_H

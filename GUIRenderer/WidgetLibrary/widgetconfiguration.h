#ifndef WIDGETCONFIGURATION_H
#define WIDGETCONFIGURATION_H

#include "widgetposition.h"


class WidgetConfiguration
{
public:
    WidgetConfiguration();
    virtual ~WidgetConfiguration() {}

    WidgetPosition *position;
};

#endif // WIDGETCONFIGURATION_H

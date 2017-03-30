#ifndef KNOBCONFIGURATION_H
#define KNOBCONFIGURATION_H

#include "widgetconfiguration.h"

#include <QString>


class KnobConfiguration : public WidgetConfiguration
{
public:
    KnobConfiguration();
    virtual ~KnobConfiguration() {}

    double MinValue;
    double MaxValue;

    QString unit;
    QString TagName;

};

#endif // KNOBCONFIGURATION_H

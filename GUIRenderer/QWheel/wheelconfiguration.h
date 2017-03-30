#ifndef WHEELCONFIGURATION_H
#define WHEELCONFIGURATION_H

#include "widgetconfiguration.h"
#include <QStringList>

class WheelConfiguration : public WidgetConfiguration
{
public:
    WheelConfiguration();
    virtual ~WheelConfiguration() {}

    QString TagName;
    QStringList values;
    void setValues(QStringList items );

};

#endif // WHEELCONFIGURATION_H

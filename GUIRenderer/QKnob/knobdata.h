#ifndef KNOBDATA_H
#define KNOBDATA_H

#include "widgetdata.h"

class KnobData : public WidgetData
{
public:
    KnobData();
    KnobData(long id);



    double value;
};

#endif // KNOBDATA_H

#include "wheelconfiguration.h"

WheelConfiguration::WheelConfiguration()
{

}

void WheelConfiguration::setValues( QStringList items )
{
    if(items.isEmpty())
        return;

    this->values = items;

}

#ifndef GRAPHCONFIGURATION_H
#define GRAPHCONFIGURATION_H

#include "widgetconfiguration.h"
#include <QVector>
#include <QString>

class GraphConfiguration : public WidgetConfiguration
{
public:
    GraphConfiguration();
    virtual ~GraphConfiguration() {}

    int MinX;
    int MaxX;

    int MinY;
    int MaxY;

    QVector<QString> LabelsX;
    QVector<double> TicksX;

    QVector<double> TicksY;
    QVector<QString> LabelsY;

};

#endif // GRAPHCONFIGURATION_H

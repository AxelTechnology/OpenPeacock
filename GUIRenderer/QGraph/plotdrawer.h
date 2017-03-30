#ifndef PLOTDRAWER_H
#define PLOTDRAWER_H

#include "qcustomplot.h"

#include <QObject>

class PlotDrawer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isPlotting READ isPlotting)

public:
    //PlotDrawer(QObject * parent = 0);
    PlotDrawer();
    ~PlotDrawer();

    void Initialize(QCustomPlot* plot);    

    bool isPlotting() const { return m_isPlotting; }    

public slots:
    void startPlotting();
    void Replot();

private:
    void ReplotPrivate();

    QCustomPlot* m_plot;

    bool m_isPlotting;
    bool m_plottingRequest;
    bool m_cancellationPending;

};

#endif // PLOTDRAWER_H

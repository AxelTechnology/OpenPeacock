#include "plotdrawer.h"
#include "logger.h"
#include <QtConcurrent/QtConcurrent>

//PlotDrawer::PlotDrawer(QObject *parent ) : QObject( parent )
PlotDrawer::PlotDrawer() : QObject( )
{
    m_isPlotting = false;
    m_cancellationPending = false;
    m_plottingRequest = false;
    m_plot = 0;
}


PlotDrawer::~PlotDrawer()
{

}


void PlotDrawer::Initialize(QCustomPlot* plot)
{
    m_plot = plot;
}

void PlotDrawer::startPlotting()
{
    while(true)
    {
        if(m_cancellationPending)
            break;
        QThread::msleep(500);
        if(m_isPlotting)
            continue;
        if(!m_plottingRequest)
            continue;

        ReplotPrivate();
    }
}


void PlotDrawer::Replot()
{
    if(m_plot == 0)
        return;

    m_plottingRequest = true;

    //ReplotPrivate();
    //QFuture<void> ret = QtConcurrent::run(this, &PlotDrawer::ReplotPrivate);
    //m_plot->replot();

}


void PlotDrawer::ReplotPrivate()
{
//    if(m_isPlotting)
//    {
//        Logger::Instance()->AddLog("still plotting");
//        return;
//    }

    m_isPlotting = true;
    //Logger::Instance()->AddLog("isplotting=true");
    m_plot->replot();
    //QThread::sleep(1000);
    m_isPlotting = false;
    m_plottingRequest = false;
    //Logger::Instance()->AddLog("isplotting=false");
}


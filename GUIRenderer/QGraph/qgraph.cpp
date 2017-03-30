
#include "qgraph.h"
#include <QtGlobal>
#include <QtGui>
#include <QPainter>
#include <QFont>
#include "logger.h"
#include "jsonserializer.h"


QGraph::QGraph(QWidget *parent)
    : AbstractWidget(parent)
{
    filter = new BiQuadFilter();

    QVBoxLayout* layout = new QVBoxLayout(this);
    //_graphData = 0;
    this->setLayout(layout);

    layout->setMargin(0);

    _redrawRequest = false;
    _replotRequest = false;

    m_background = QColor(47, 47, 47);
    m_foreground = QColor(Qt::white);
    m_lineColor = QColor("Yellow");
    m_dotsColor = QColor(253, 0, 0, 200);

    m_colorLines.append(QColor(253, 0 ,0));
    m_colorLines.append(QColor(0, 253, 0));
    m_colorLines.append(QColor(0, 0, 253));
    m_colorLines.append(QColor(163, 0, 251));
    m_colorLines.append(QColor(150, 215, 234));


    customPlot = new QCustomPlot(this);
    layout->addWidget(customPlot);


//    _plotDrawerThread = new QThread();
//    _plotDrawerThread->setObjectName("AxelGUI_Plotdrawer");
//    _plotDrawer = new PlotDrawer();
//    _plotDrawer->Initialize(customPlot);
//    _plotDrawer->moveToThread(_plotDrawerThread);
//    connect(_plotDrawerThread, SIGNAL(started()), _plotDrawer, SLOT(startPlotting()));
//    connect(_plotDrawer, SIGNAL(finished()), _plotDrawerThread, SLOT(quit()));
//    connect(_plotDrawerThread, SIGNAL(finished()), _plotDrawerThread, SLOT(deleteLater()));
    //_plotDrawerThread->start();

    // Drawing the graph from another thread does not work, using a timer instead :-(
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(UpdateTimeout()));
    _timer->start(30);

    configuration = new GraphConfiguration();
    configuration->LabelsX << "10" << "100" << "1k" << "10k";
    configuration->LabelsY << "-12" << "-9" << "-6" << "-3" << "0" << "3" << "6" << "9" << "12";
    configuration->MaxX = 24000;
    configuration->MinX = 10;
    configuration->MaxY = 12;
    configuration->MinY  = -12;
    configuration->TicksX << 10 << 100 << 1000 << 10000 << 100000;
    configuration->TicksY << -12 << -9 << -6 << -3 << 0 << 3 << 6 << 9 << 12;
    configure(*configuration);

    RecalculateData(false);

    setMinimumSize(150, 100);

    _firstPlot.start();

}

void QGraph::UpdateTimeout()
{
    if(customPlot == 0)
        return;
    if(_redrawRequest)
    {
        _redrawRequest = false;
        customPlot->replot();
    }
    else if(_replotRequest)
    {
        _replotRequest = false;
        ReplotData();
    }
}


void QGraph::configure(GraphConfiguration &configuration){

    //customPlot->setGeometry(*configuration.position->pos);

    setBackground(m_background);

    if(customPlot->graphCount() == 0)
        customPlot->addGraph();


    QPen pen;
    pen.setColor(m_lineColor);
    pen.setWidth(2);

    customPlot->graph(0)->setPen(pen);
    customPlot->graph(0)->setName("x");

    setLabelXY(configuration);
    setRangeXY(configuration);

    customPlot->yAxis->grid()->setSubGridVisible(false);
    customPlot->xAxis->grid()->setSubGridVisible(true);

    customPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    customPlot->xAxis->setScaleLogBase(10);

    customPlot->xAxis->setSubTickCount(8);
    customPlot->yAxis->setSubTickCount(8);

    customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));    
    QPen penGridLines(Qt::DashLine);
    penGridLines.setColor(QColor(254,254,254,50));
    customPlot->xAxis->grid()->setSubGridPen(penGridLines);
    customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
    customPlot->yAxis->setTickPen(QPen(Qt::DotLine));


    setForeground(m_foreground);

    customPlot->setContentsMargins(0,0,0,0);

    QFont legendFont = font();
    legendFont.setPointSize(9);
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setBrush(QBrush(QColor(255,255,255,100)));
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);
    customPlot->setAutoAddPlottableToLegend(false);
    //customPlot->legend->setVisible(true);
    customPlot->legend->setBorderPen(Qt::NoPen);
    customPlot->legend->setMinimumSize(0, 0);
    customPlot->legend->setMinimumMargins(QMargins(0,0,0,0));
    customPlot->legend->setMargins(QMargins(1,1,1,1));

    //customPlot->replot();
    //_plotDrawer->Replot();
    _redrawRequest = true;
}



QGraph::~QGraph()
{
    //_plotDrawerThread->quit();
    _graphData.clear();
    //delete ;

    if(_timer != 0)
    {
        _timer->stop();
        delete _timer;
    }
}


void QGraph::RecalculateData(bool showAnimation)
{

    //Debug --
//    GraphData * g1 = new GraphData();
//    g1->setFilterType(BiQuadFilterType_lp);
//    g1->setSampleRate(48000);
//    g1->setSelected(true);
//    g1->setCoeffA0(0.00414239650255865);
//    g1->setCoeffA1(0.0082847930051173);
//    g1->setCoeffA2(0.00414239650255865);
//    g1->setCoeffB1(-1.920229656436938);
//    g1->setCoeffB2(0.9367992424471726);

//    GraphData * g2 = new GraphData();
//    g2->setFilterType(BiQuadFilterType_peak);
//    g2->setSampleRate(48000);
//    g2->setGain(55);
//    g2->setSelected(false);
//    g2->setCoeffA0(2.1597034025262833);
//    g2->setCoeffA1(-1.991594792155031);
//    g2->setCoeffA2(-0.1638353035657551);
//    g2->setCoeffB1(-1.991594792155031);
//    g2->setCoeffB2(0.995868098960528);

//    _graphData.append(g1);
//    _graphData.append(g2);
    // ---

    if(_graphData.isEmpty())
        return;

    RecalculateBounds();

    foreach(GraphData* graph , _graphData)
    {
        graph->valueX()->clear();
        graph->valueY()->clear();

        filter->SetFilterCoefficients(graph->coeffA0(), graph->coeffA1(), graph->coeffA2(), graph->coeffB0(), graph->coeffB1(), graph->coeffB2());

        Logger::Instance()->AddLog("Setting graph Xs", LoggerLevel_DEBUGVERBOSE);

        double i = 10;
        while(i < configuration->MaxX)
        {
            graph->valueX()->append(i);            
            if (i < 20)
                i += 0.5;
            else if(i< 100)
                i += 1;
            else if (i < 200)
                i += 5;
            else if(i< 1000)
                i += 10;
            else if(i< 2000)
                i += 50;
            else
                i += 100;
        }

        Logger::Instance()->AddLog("Setting graph Ys", LoggerLevel_DEBUGVERBOSE);

        for(int j = 0; j < graph->valueX()->count(); j++){
            double val = filter->GetSample(graph->valueX()->at(j), 48000);// (double) configuration->MaxX);
            graph->valueY()->append(val);
        }
    }

    CalculateAverageData();   

    _replotRequest = true;
    //ReplotData();

    if(_firstPlot.elapsed() > 500)
        StartAnimation();

}


void QGraph::CalculateAverageData()
{
    _graphDataMeanX.clear();
    _graphDataMeanY.clear();

    if(_graphData.count() == 0)
        return;

    //get the graph with the maximum number of X elements
    int iMaxX = 0;
    int maxX = 0;
    for(int i = 0; i < _graphData.count(); i++)
    {
        if(maxX < _graphData.at(i)->valueX()->count())
        {
            iMaxX = i;
            maxX = _graphData.at(i)->valueX()->count();
        }
    }

    //Cycle every X value
    for(int j = 0; j < maxX; j++)
    {
        _graphDataMeanX.append(_graphData.at(iMaxX)->valueX()->at(j));
        double meanY = 0;

        //Calculate the average Y value o every graph
        for(int k = 0; k < _graphData.count(); k++)
        {
            if(j >= _graphData.at(k)->valueY()->count())
                continue;
            meanY += _graphData.at(k)->valueY()->at(j);
        }
        //meanY = meanY / _graphData.count();

        _graphDataMeanY.append(meanY);
    }

//    QString meanDebug("Graph sum line debug: \n");
//    for (int i = 0; i < _graphDataMeanX.count(); i++)
//    {
//        meanDebug.append("x=" + QString::number(_graphDataMeanX.at(i)) + "y=" + QString::number(_graphDataMeanY.at(i)) + "\n");
//    }
//    Logger::Instance()->AddLog(meanDebug, LoggerLevel_DEBUG);
}



void QGraph::RecalculateBounds()
{
    Logger::Instance()->AddLog("Starting recalculating Graph data", LoggerLevel_DEBUG);

    //replaced by static bounds
    /*
    double maxSR = 0;
    configuration->MinY = -10;
    configuration->MaxY = 10;
    foreach(GraphData* graph , _graphData)
    {
        if(graph->sampleRate() > maxSR)
            maxSR = graph->sampleRate();


        if((graph->filterType() == BiQuadFilterType_hp) || (graph->filterType() == BiQuadFilterType_lp) || (graph->filterType() == BiQuadFilterType_bp_0db) ||
                (graph->filterType() == BiQuadFilterType_bp_peak) || (graph->filterType() == BiQuadFilterType_notch))
        {
            if(configuration->MaxY < 20)
                configuration->MaxY = 20;
            if (configuration->MinY > -100)
                configuration->MinY  = -100;
            //configuration->TicksY << -100 << -75 << -50 << -25 << 0 << 25;
        }
        else
        {
            if(graph->gain() >= 10)
            {
                if(graph->gain() < 20)
                {
                    if(configuration->MaxY < 20)
                        configuration->MaxY = 20;
                }
                else if(graph->gain() < 40)
                {
                    if(configuration->MaxY < 40)
                        configuration->MaxY = 40;
                }
                else
                {
                    if(configuration->MaxY < 100)
                        configuration->MaxY = 100;
                }
            }
            else if(graph->gain() <= -10)
            {
                if(graph->gain() > -20)
                {
                    if(configuration->MinY > -20)
                        configuration->MinY = -20;
                }
                else if(graph->gain() > -40)
                {
                    if(configuration->MinY > -40)
                        configuration->MinY = -40;
                }
                else
                {
                    if(configuration->MinY > -100)
                        configuration->MinY = -100;
                }
            }
        }
    }

    configuration->TicksY.clear();
    configuration->LabelsY.clear();
    configuration->TicksY << configuration->MinY << configuration->MinY * 3/4 << configuration->MinY * 1/2 << configuration->MinY * 1/4 << 0 << configuration->MaxY * 1/4 << configuration->MaxY * 1/2 << configuration->MaxY * 3/4 << configuration->MaxY ;
    foreach(double y, configuration->TicksY)
        configuration->LabelsY << QString::number(y);

    if(maxSR <= 0)
        return;

    configuration->TicksX.clear();
    configuration->LabelsX.clear();
    configuration->MaxX = maxSR / 2 * 1.1;
    configuration->MinX = maxSR / 2000;
    configuration->TicksX << maxSR / 2 / 1000  << maxSR / 2 / 100 << maxSR / 2 / 10 << maxSR / 2 ;
    configuration->LabelsX << QString::number(configuration->TicksX.at(0)) << QString::number(configuration->TicksX.at(1)) <<
                              QString::number(configuration->TicksX.at(2)) << QString::number(configuration->TicksX.at(3));

*/

    setLabelXY(*configuration);
    setRangeXY(*configuration);
}


void QGraph::setLabelXY(GraphConfiguration &configuration){



    customPlot->xAxis->setAutoTicks(false);
    customPlot->xAxis->setAutoTickLabels(false);
    customPlot->xAxis->setTickVector(configuration.TicksX);
    customPlot->xAxis->setTickVectorLabels(configuration.LabelsX);


    customPlot->yAxis->setAutoTicks(false);
    customPlot->yAxis->setAutoTickLabels(false);
    customPlot->yAxis->setTickVector(configuration.TicksY);
    customPlot->yAxis->setTickVectorLabels(configuration.LabelsY);


}


void QGraph::setRangeXY(GraphConfiguration &configuration){

     customPlot->xAxis->setRange(configuration.MinX, configuration.MaxX);
     customPlot->yAxis->setRange(configuration.MinY, configuration.MaxY);

}


void QGraph::ReplotData()
{    
    Logger::Instance()->AddLog("Updating graph data", LoggerLevel_DEBUG);

    customPlot->clearGraphs();
    QVector<double> pointsX;
    QVector<double> pointsY;

    //Plot eq lines
    for(int i = 0; i< _graphData.count() ; i++)
    {
        GraphData * data = _graphData.at(i);
        if(data == 0)
            continue;
        if(data->valueX() == 0)
            continue;
        if(data->valueY() == 0)
            continue;

        while(i >= customPlot->graphCount())
            customPlot->addGraph();

        customPlot->graph(i)->setData(*data->valueX(), *data->valueY());
        if(i < m_colorLines.count())
        {
            QPen p(QColor(SetAlpha(m_colorLines.at(i), 200)));
            p.setWidth(1);
            customPlot->graph(i)->setPen(p);
            customPlot->graph(i)->setBrush(QBrush(SetAlpha(m_colorLines.at(i), 50)));
        }

        customPlot->graph(i)->setName("G=" + QString::number(_graphData.at(i)->gain(), 'f', 1) + " f=" + QString::number(_graphData.at(i)->frequency(), 'f', 1));
        customPlot->graph(i)->addToLegend();

        pointsX.append(data->frequency());
        pointsY.append(data->gain());
    }

    //Plot eq points
    for (int j = 0; j < pointsX.count(); j++)
    {
        customPlot->addGraph();
        if(j < m_colorLines.count())
            customPlot->graph(customPlot->graphCount() - 1)->setPen(m_colorLines.at(j));
        customPlot->graph(customPlot->graphCount() - 1)->setLineStyle(QCPGraph::lsNone);
        customPlot->graph(customPlot->graphCount() - 1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 6));
        if((j >= pointsX.count()) || (j >= pointsY.count()))
            break;
        QVector<double> px;
        px.append(pointsX.at(j));
        QVector<double> py;
        py.append(pointsY.at(j));
        customPlot->graph(customPlot->graphCount() - 1)->setData(px, py);
    }


    //Plot average line
    customPlot->addGraph();
    int iAvPlot = customPlot->graphCount() - 1;
    customPlot->graph(iAvPlot)->setData(_graphDataMeanX, _graphDataMeanY);
    QPen penAv(m_lineColor);
    penAv.setWidth(2);
    customPlot->graph(iAvPlot)->setPen(penAv);

    customPlot->replot();

    //if(_plotDrawer->isPlotting())
        //return;
    //_plotDrawer->Replot();
    //QMetaObject::invokeMethod(_plotDrawer, "Replot"); // SLOT(Replot));
    //_plotDrawer->Replot();

    Logger::Instance()->AddLog("Finished recalculating Graph data", LoggerLevel_DEBUG);
}


QColor QGraph::SetAlpha(QColor c, int alpha)
{
    return QColor(c.red(), c.green(), c.blue(), alpha);
}



void QGraph::setForeground(QColor newForeColor)
{
    m_foreground=newForeColor;

    customPlot->yAxis->setTickPen(QPen(m_foreground));
    customPlot->yAxis->setBasePen(QPen(m_foreground));
    customPlot->xAxis->setTickPen(QPen(m_foreground));
    customPlot->xAxis->setSubTickPen(QPen(m_foreground));
    customPlot->yAxis->setSubTickPen(QPen(m_foreground));
    customPlot->xAxis->setBasePen(QPen(m_foreground));
    customPlot->yAxis->setTickLabelColor(m_foreground);
    customPlot->xAxis->setTickLabelColor(m_foreground);

    //customPlot->replot();
    //_plotDrawer->Replot();
    _redrawRequest = true;
}


void QGraph::setBackground(QColor newBackColor)
{
    m_background=newBackColor;
    customPlot->setBackground(m_background);

    //customPlot->replot();
    //_plotDrawer->Replot();
    _redrawRequest = true;
}


void QGraph::AnimationFinished()
{
    if(_animationStarted)
    {
        customPlot->legend->setVisible(true);
        //customPlot->replot();
        //_plotDrawer->Replot();
        _redrawRequest = true;
    }
}
void QGraph::AnimationStarted()
{
    if(!_animationStarted)
    {
        customPlot->legend->setVisible(false);
        //customPlot->replot();
        //_plotDrawer->Replot();
        _redrawRequest = true;
    }
}


bool QGraph::ManageRemoteCommand(QString propertyName, QJsonValue value, bool init, QString * error)
{
    Q_UNUSED(error);
    bool retVal = AbstractWidget::ManageRemoteCommand(propertyName, value, init, error);

    if (value == 0)
    {
        error = new QString("Error setting QGraph " + this->objectName() + " property=" + propertyName + " value=" + value.toString() );
        return false;
    }

    if(propertyName == "graphs")
    {        
        int graphsReceived = 0;

        if (!value.isArray())
        {
            error = new QString("Error setting QGraph " + this->objectName() + " property=" + propertyName + " value=" + value.toString() );
            return false;
        }

        //_graphData.clear();

        QJsonArray graphs = value.toArray();
        //foreach (QJsonValue v, graphs) {
        for(int i = 0; i < graphs.count(); i++){
            QJsonValue v = graphs.at(i);
            GraphData * gData = new GraphData();
            bool validGraphObject = false;

            //Valid data graphdata
            if((v.isObject()) && (!v.isNull()))
            {
                QJsonObject obj = v.toObject();

                if(obj.count() > 0)
                {
                    validGraphObject = true;
                    graphsReceived ++;
                    gData->SetJsonData(&obj);

                    //if the data position is over the bounds of the current array add an element, otherwise replace
                    if(i >= _graphData.count())
                        _graphData.append(gData);
                    else
                        _graphData.replace(i, gData);
                }
            }

            //NULL data
            if(!validGraphObject)
            {
                //If I have not yet received a valid data for this position, add blank data
                if(i >= _graphData.count())
                    _graphData.append(gData);
            }

        }

        if(graphsReceived == 1)
            RecalculateData(true);        
    }

    return retVal;
}


void QGraph::SetJsonData(QJsonObject *data)
{
    if (data == 0)
        return;
    AbstractWidget::SetJsonData(data);

    m_background = GetColorFromString((*data)["backColor"].toString());
    m_foreground = GetColorFromString((*data)["foreColor"].toString());

    m_lineColor = GetColorFromString((*data)["lineColor"].toString());
    m_dotsColor = GetColorFromString((*data)["dotsColor"].toString());


    RecalculateData(false);

}



QJsonObject* QGraph::GetJsonData()
{
    QJsonObject* retObj = AbstractWidget::GetJsonData();

    retObj->insert("backColor", QJsonValue(GetStringFromColor(m_background)));
    retObj->insert("foreColor", QJsonValue(GetStringFromColor(m_foreground)));

    retObj->insert("lineColor", QJsonValue(GetStringFromColor(m_lineColor)));
    retObj->insert("dotsColor", QJsonValue(GetStringFromColor(m_dotsColor)));

//    retObj->insert("filterType", QJsonValue((int)m_filterType));
//    retObj->insert("sampleRate", QJsonValue(m_sampleRate));
//    retObj->insert("gain", QJsonValue(m_gain));
//    retObj->insert("A0", QJsonValue(m_coeffA0));
//    retObj->insert("A1", QJsonValue(m_coeffA1));
//    retObj->insert("A2", QJsonValue(m_coeffA2));
//    retObj->insert("B1", QJsonValue(m_coeffB1));
//    retObj->insert("B2", QJsonValue(m_coeffB2));


//    if(_graphData)
//    {
//        QJsonArray valueX = QJsonArray();
//        if(_graphData->ValueX != 0)
//        {
//            for (int i = 0; i < _graphData->ValueX->count(); i++) {
//                valueX.append(QJsonValue(_graphData->ValueX->at(i)));
//            }
//            retObj->insert("valueX", QJsonValue(valueX));
//        }

//        if(_graphData->ValueY != 0)
//        {
//            QJsonArray valueY = QJsonArray();
//            for (int i = 0; i < _graphData->ValueY->count(); i++){
//                valueY.append(QJsonValue(_graphData->ValueY->at(i)));
//            }
//            retObj->insert("valueY", QJsonValue(valueY));
//        }
//    }
    return retObj;
}

QJsonObject *QGraph::GetJsonDataVars()
{
    QJsonObject* retObj = AbstractWidget::GetJsonDataVars();
    QJsonArray properties;
    if(retObj->contains("properties"))
        properties = (*retObj)["properties"].toArray();
    else
        properties = QJsonArray();

    QJsonObject value = JsonSerializer::GetVarsProperty(_controllerDataValue, "GRAPHDATA", "graphs");
    if(!value.isEmpty())
        properties.append(value);

    if(properties.isEmpty())
        return 0;

    retObj->insert("properties", properties);
    return retObj;
}




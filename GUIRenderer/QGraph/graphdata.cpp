#include "graphdata.h"
#include <QJsonObject>

GraphData::GraphData()
{
    m_valueX = 0;
    m_valueY = 0;

    m_filterType = BiQuadFilterType_lp;
    m_sampleRate = 48000;
    m_gain = 0;
    m_frequency = 0;
    m_coeffA0 = 1;
    m_coeffA1 = 0;
    m_coeffA2 = 0;
    m_coeffB0 = 1;
    m_coeffB1 = 0;
    m_coeffB2 = 0;
    m_selected = false;
}


void GraphData::Init()
{
    m_valueX = new QVector<double>();
    m_valueY = new QVector<double>();
}


void GraphData::SetJsonData(QJsonObject *data)
{
    if (data == 0)
        return;
    setFilterType((BiQuadFilterType)(*data)["filtertype"].toInt());
    setSampleRate((*data)["samplerate"].toDouble());
    setGain((*data)["gain"].toDouble());    
    setSelected((*data)["selected"].toBool());
    setFrequency((*data)["frequency"].toDouble());
    if((*data).contains("A0"))
        setCoeffA0((*data)["A0"].toDouble());
    setCoeffA1((*data)["A1"].toDouble());
    setCoeffA2((*data)["A2"].toDouble());
    if((*data).contains("B0"))
        setCoeffB0((*data)["B0"].toDouble());
    setCoeffB1((*data)["B1"].toDouble());
    setCoeffB2((*data)["B2"].toDouble());
}

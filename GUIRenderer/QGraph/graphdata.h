#ifndef GRAPHDATA_H
#define GRAPHDATA_H

#include "widgetdata.h"
#include "biquadfilter.h"
#include <QVector>


class GraphData : public WidgetData
{
    Q_PROPERTY(bool selected READ selected WRITE setSelected)

    Q_PROPERTY(double coeffA0 READ coeffA0 WRITE setCoeffA0)
    Q_PROPERTY(double coeffA1 READ coeffA1 WRITE setCoeffA1)
    Q_PROPERTY(double coeffA2 READ coeffA2 WRITE setCoeffA2)
    Q_PROPERTY(double coeffB0 READ coeffB0 WRITE setCoeffB0)
    Q_PROPERTY(double coeffB1 READ coeffB1 WRITE setCoeffB1)
    Q_PROPERTY(double coeffB2 READ coeffB2 WRITE setCoeffB2)

    Q_PROPERTY(double sampleRate READ sampleRate WRITE setSampleRate)
    Q_PROPERTY(double gain READ gain WRITE setGain)
    Q_PROPERTY(double frequency READ frequency WRITE setFrequency)
    Q_PROPERTY(BiQuadFilterType filterType READ filterType WRITE setFilterType)

    Q_PROPERTY(QVector<double> * valueX READ valueX)
    Q_PROPERTY(QVector<double> * valueY READ valueY)

public:
     GraphData();

     void Init();

     void SetJsonData(QJsonObject *data);

     double coeffA0() const { return m_coeffA0;}
     void setCoeffA0(double coeffA0) { m_coeffA0 = coeffA0; }

     double coeffA1() const { return m_coeffA1; }
     void setCoeffA1(double coeffA1) { m_coeffA1 = coeffA1; }

     double coeffA2() const { return m_coeffA2; }
     void setCoeffA2(double coeffA2) { m_coeffA2 = coeffA2; }

     double coeffB0() const { return m_coeffB0; }
     void setCoeffB0(double coeffB0) { m_coeffB0 = coeffB0; }

     double coeffB1() const { return m_coeffB1; }
     void setCoeffB1(double coeffB1) { m_coeffB1 = coeffB1; }

     double coeffB2() const { return m_coeffB2; }
     void setCoeffB2(double coeffB2) { m_coeffB2 = coeffB2; }

     double sampleRate() const { return m_sampleRate; }
     void setSampleRate(double sampleRate) { m_sampleRate = sampleRate; }

     double gain() const { return m_gain; }
     void setGain(double gain) { m_gain = gain; }

     BiQuadFilterType filterType() const { return m_filterType; }
     void setFilterType(BiQuadFilterType filterType) { m_filterType = filterType; }

     QVector<double> * valueX()
     {
         if(m_valueX == 0)
            m_valueX = new QVector<double>();
         return m_valueX;
     }

     QVector<double> * valueY()
     {
         if(m_valueY == 0)
            m_valueY = new QVector<double>();
         return m_valueY;
     }


     bool selected() const { return m_selected; }
     void setSelected(bool selected) { m_selected = selected; }


     double frequency() const { return m_frequency; }
     void setFrequency(double frequency) { m_frequency = frequency; }

public slots:


private:
     QVector<double> * m_valueX;
     QVector<double> * m_valueY;


     double m_coeffA0;
     double m_coeffA1;
     double m_coeffA2;
     double m_coeffB0;
     double m_coeffB1;
     double m_coeffB2;
     double m_sampleRate;
     double m_gain;
     BiQuadFilterType m_filterType;

     bool m_selected;
     double m_frequency;
};

#endif // GRAPHDATA_H

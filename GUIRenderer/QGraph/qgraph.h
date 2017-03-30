
#ifndef QGraph_H
#define QGraph_H

#include <Qt>
#include <QWidget>
#include <QPropertyAnimation>
#include "biquadfilter.h"
#include "plotdrawer.h"
#include "qcustomplot.h"
#include <graphdata.h>
#include <graphconfiguration.h>
#include <abstractwidget.h>


class QColor;


class QGraph : public AbstractWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor foreground READ foreground WRITE setForeground)
    Q_PROPERTY(QColor background READ background WRITE setBackground)
    Q_PROPERTY(QColor lineColor READ lineColor WRITE setLineColor)
    Q_PROPERTY(QColor dotsColor READ dotsColor WRITE setDotsColor)

    Q_PROPERTY(QString controllerDataValue READ controllerDataValue WRITE setControllerDataValue)


public:

/*!
  \brief Constructor
  \param parent Parent Widget
*/
    QGraph(QWidget *parent = 0);

    QGraph(long id=0);




    void ReplotData();

    void configure(GraphConfiguration &configuration);

    bool ManageRemoteCommand(QString propertyName, QJsonValue value, bool init, QString * error) Q_DECL_OVERRIDE;
    void SetJsonData(QJsonObject* data) Q_DECL_OVERRIDE;
    QJsonObject* GetJsonData() Q_DECL_OVERRIDE;
    QJsonObject* GetJsonDataVars() Q_DECL_OVERRIDE;

    /*!
      \brief Destructor
      \param none
    */
        ~QGraph();

    void setRangeXY(GraphConfiguration &configuration);


    void setLabelXY(GraphConfiguration &configuration);




    /*!
      \return The foreground color used for the widget
    */
        QColor foreground() const { return m_foreground; }

    /*!
      \return The background color used for the widget
    */
        QColor background() const { return m_background; }


        QColor lineColor() const { return m_lineColor; }
        void setLineColor(QColor lineColor) { m_lineColor = lineColor; }

        void RecalculateData(bool showAnimation);
        void RecalculateBounds();        


signals:

/*!
  \signals This signal is emitted to report an error condition.
*/
    void errorSignal(int);



public slots:

/*!
  \slots This slot is used to set the widget foreground color.
*/
    void setForeground(QColor);
/*!
  \slots This slot is used to set the widget background color.
*/
    void setBackground(QColor);

protected:

    void AnimationFinished() Q_DECL_OVERRIDE;
    void AnimationStarted() Q_DECL_OVERRIDE;



public:
    QCustomPlot *customPlot;
    GraphConfiguration *configuration;


    QString controllerDataValue() const
    {
        return _controllerDataValue;
    }
    void setControllerDataValue(QString controllerValueId)
    {
        _controllerDataValue = controllerValueId;
    }


    QColor dotsColor() const { return m_dotsColor; }
    void setDotsColor(QColor dotsColor) { m_dotsColor = dotsColor; }

private slots:
    void UpdateTimeout();

private:
    void CalculateAverageData();
    QColor SetAlpha(QColor c, int alpha);

    QColor m_foreground;
    QColor m_background;
    QColor m_lineColor;

    QList<QColor> m_colorLines;


    QList<GraphData *> _graphData;
    QVector<double> _graphDataMeanX;
    QVector<double> _graphDataMeanY;

    BiQuadFilter * filter;


    QString _controllerDataValue;
    QColor m_dotsColor;

    QElapsedTimer _firstPlot;

    //PlotDrawer* _plotDrawer;
    //QThread* _plotDrawerThread;
    QTimer* _timer;
    bool _redrawRequest;
    bool _replotRequest;

};


#endif // QGraph_H

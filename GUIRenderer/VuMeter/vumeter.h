#ifndef QVUMETER_H
#define QVUMETER_H

#include "abstractwidget.h"

#include <QElapsedTimer>
#include <QTimer>
#include <QWidget>
#include <QPainter>

class VuMeter : public AbstractWidget
{
    Q_OBJECT
    Q_PROPERTY(double valueA READ valueA WRITE setValueA)
    Q_PROPERTY(double valueAPerc READ valueAPerc WRITE setValueAPerc)
    Q_PROPERTY(double valueB READ valueB WRITE setValueB)
    Q_PROPERTY(double valueBPerc READ valueBPerc WRITE setValueBPerc)
    Q_PROPERTY(double maxValue READ maxValue WRITE setMaxValue)
    Q_PROPERTY(double color1MaxValue READ color1MaxValue WRITE setColor1MaxValue)
    Q_PROPERTY(double color2MaxValue READ color2MaxValue WRITE setColor2MaxValue)

    Q_PROPERTY(QColor color1 READ color1 WRITE setColor1)
    Q_PROPERTY(QColor color2 READ color2 WRITE setColor2)
    Q_PROPERTY(QColor color3 READ color3 WRITE setColor3)

    Q_PROPERTY(int decayValuePerSec READ decayValuePerSec WRITE setDecayValuePerSec)

    //Q_PROPERTY(QColor colorOff READ colorOff WRITE setColorOff)

    Q_PROPERTY(QString controllerDataValueA READ controllerDataValueA WRITE setControllerDataValueA)
    Q_PROPERTY(QString controllerDataValueAPerc READ controllerDataValueAPerc WRITE setControllerDataValueAPerc)
    Q_PROPERTY(QString controllerDataValueB READ controllerDataValueB WRITE setControllerDataValueB)
    Q_PROPERTY(QString controllerDataValueBPerc READ controllerDataValueBPerc WRITE setControllerDataValueBPerc)
    Q_PROPERTY(QString controllerDataNumVuMeters READ controllerDataNumVuMeters WRITE setControllerDataNumVuMeters)

    Q_PROPERTY(int numVuMeters READ numVuMeters WRITE setNumVuMeters)
    Q_PROPERTY(QStringList labels READ labels WRITE setLabels)
    Q_PROPERTY(bool showLabelsLeft READ showLabelsLeft WRITE setShowLabelsLeft)

public:

    VuMeter(QWidget *parent = 0);
    virtual ~VuMeter();
    void resizeEvent(QResizeEvent * event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *e);
    bool ManageRemoteCommand(QString propertyName, QJsonValue valueA, bool init, QString * error) Q_DECL_OVERRIDE;
    void SetJsonData(QJsonObject* data) Q_DECL_OVERRIDE;
    QJsonObject* GetJsonData() Q_DECL_OVERRIDE;
    QJsonObject* GetJsonDataVars() Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);

    double valueA(){return _valueA;}
    void setValueA(double val);
    double valueAPerc() const { return _valueAPerc; }
    void setValueAPerc(double valueAPerc);
    double valueB(){return _valueB;}
    void setValueB(double val);
    double valueBPerc() const { return _valueBPerc; }
    void setValueBPerc(double valueBPerc);
    double maxValue(){return _maxValue;}
    void setMaxValue(double val) { _maxValue = val; update();}
    double color1MaxValue(){return _color1MaxValue;}
    void setColor1MaxValue(double val) {val > 0 ? _color1MaxValue = val : _color1MaxValue = 0 ; update();}
    double color2MaxValue(){return _color2MaxValue;}
    void setColor2MaxValue(double val)
    {
        val > 0 ? _color2MaxValue = val : _color2MaxValue = 0;
        update();
    }
    QColor color1(){ return _color1;}
    void setColor1(QColor value);
    QColor color2(){ return _color2;}
    void setColor2(QColor value);
    QColor color3(){ return _color3;}
    void setColor3(QColor value);

    int decayValuePerSec() const { return m_decayValuePerSec; }
    void setDecayValuePerSec(int decayMsec) { m_decayValuePerSec = decayMsec; }


    QString controllerDataValueA() const { return _controllerDataValueA; }
    void setControllerDataValueA(QString controllerValueId) { _controllerDataValueA = controllerValueId; }

    QString controllerDataValueAPerc() const { return _controllerDataValueAPerc; }
    void setControllerDataValueAPerc(QString controllerValuePercId) { _controllerDataValueAPerc = controllerValuePercId; }

    QString controllerDataValueB() const { return _controllerDataValueB; }
    void setControllerDataValueB(QString controllerValueBId) { _controllerDataValueB = controllerValueBId; }

    QString controllerDataValueBPerc() const { return _controllerDataValueBPerc; }
    void setControllerDataValueBPerc(QString controllerValuePercId) { _controllerDataValueBPerc = controllerValuePercId; }


    int numVuMeters() const { return _numVuMeters; }
    void setNumVuMeters(int numVuMeters) { _numVuMeters = numVuMeters; update(); }

    QString controllerDataNumVuMeters() const { return _controllerDataNumVuMeters; }
    void setControllerDataNumVuMeters(QString controllerNumVuMetersId) { _controllerDataNumVuMeters = controllerNumVuMetersId; }

    QStringList labels() const { return _labels; }
    void setLabels(QStringList labels) { _labels = labels; }

    bool showLabelsLeft() const { return _showLabelsLeft; }
    void setShowLabelsLeft(bool showLabelsLeft) { _showLabelsLeft = showLabelsLeft; update(); }


signals:


private slots:

    void updateTimerTick();


private:

    bool SetJsonLabels(QJsonValue* values);
    void drawText(QPainter & painter, qreal x, qreal y, int flags, const QString & text, QRectF * boundingRect = 0, QFont * font = 0);
    void drawText(QPainter & painter, qreal x, qreal y, Qt::Alignment flags, const QString & text, QRectF * boundingRect = 0);
    double _valueA;
    double _valueAPerc;
    double _paintValueA;
    double _valueB;
    double _valueBPerc;
    double _paintValueB;
    double _maxValue;
    double _color1MaxValue;
    double _color2MaxValue;
    QColor _color1;
    QColor _color2;
    QColor _color3;
    QColor _color1off;
    QColor _color2off;
    QColor _color3off;
    double _ledAttenuation;
    int _paintWidth;
    int _paintHeight;


    QString _controllerDataValueA;
    QString _controllerDataValueAPerc;
    QString _controllerDataValueB;
    QString _controllerDataValueBPerc;
    QString _controllerDataNumVuMeters;

    int m_decayValuePerSec;
    QTimer *timer;
    QElapsedTimer _elapsedTimerA;
    QElapsedTimer _elapsedTimerB;

    int _numVuMeters;    
    QStringList _labels;
    QFont _labelsFont;
    int _labelMargin;
    bool _showLabelsLeft;
};

#endif // QVUMETER_H

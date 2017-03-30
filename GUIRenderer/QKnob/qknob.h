
#ifndef QKnob_H
#define QKnob_H

#include <Qt>
#include <QWidget>
#include <QLCDNumber>
#include <QPropertyAnimation>
#include <abstractwidget.h>


//class QColor;


class QKnob : public AbstractWidget
{
 Q_OBJECT
    Q_ENUMS(errorCode)
    Q_ENUMS(QKnobMode)
    Q_PROPERTY(double value READ value WRITE setValue)
    Q_PROPERTY(double instantValue READ instantValue WRITE setInstantValue)
    Q_PROPERTY(double minValue READ minValue WRITE setMinValue)
    Q_PROPERTY(double maxValue READ maxValue WRITE setMaxValue)
    Q_PROPERTY(double stepValue READ stepValue WRITE setStepValue)
    Q_PROPERTY(QString units READ units WRITE setUnits)

    Q_PROPERTY(bool showValueSign READ showValueSign WRITE setShowValueSign)

    Q_PROPERTY(QString leftLabel READ leftLabel WRITE setLeftLabel)
    Q_PROPERTY(QString rightLabel READ rightLabel WRITE setRightLabel)

    Q_PROPERTY(QColor foreground READ foreground WRITE setForeground)
    Q_PROPERTY(QColor background READ background WRITE setBackground)
    Q_PROPERTY(QColor editingColor READ editingColor WRITE setEditingColor)

    Q_PROPERTY(QString controllerDataValue READ controllerDataValue WRITE setControllerDataValue)
    Q_PROPERTY(QString controllerDataInstantValue READ controllerDataInstantValue WRITE setControllerDataInstantValue)

    Q_PROPERTY(int knobModeInt READ knobModeInt WRITE setKnobModeInt)
    Q_PROPERTY(QKnobMode knobMode READ knobMode WRITE setKnobMode)



public:

    enum QKnobMode
    {
        QKnobMode_INSTANTVALUE = 0,
        QKnobMode_ONLYCONFIRMEDVALUE = 1
    };

/*!
   \brief ErrorCode : This enum represents the code returned by the error signal
*/
    enum ErrorCode {MaxValueError=1,MinValueError,ThresholdError,TargetError,PrecisionError,ColorError,UnitsEmpty,OutOfRange};

/*!
  \brief Constructor
  \param parent Parent Widget
*/
    QKnob(QWidget *parent = 0);

    /*!
      \brief Destructor
      \param none
    */
        ~QKnob();

    /*!
  \return The current displayed value
*/
    double value() const { return m_value; }

/*!
  \return The minimum widget scale value
*/
    double minValue() const { return m_minValue; }

/*!
  \return The maximum widget scale value
*/
    double maxValue() const { return m_maxValue; }


/*!
  \return The Label
*/
    QString units()const { return m_units; }





/*!
  \return The foreground color used for the widget
*/
    QColor foreground() const { return m_foreground; }

/*!
  \return The background color used for the widget
*/
    QColor background() const { return m_background; }



    //void configure(KnobConfiguration &configuration);

    bool ManageRemoteCommand(QString propertyName, QJsonValue value, bool init, QString * error) Q_DECL_OVERRIDE;
    void SetJsonData(QJsonObject* data) Q_DECL_OVERRIDE;
    QJsonObject* GetJsonData() Q_DECL_OVERRIDE;
    QJsonObject* GetJsonDataVars() Q_DECL_OVERRIDE;




signals:

/*!
  \signals This signal is emitted to report an error condition.
*/
    void errorSignal(int);



public slots:
/*!
  \slots This slot is used to set QKnob value
*/
    void setValue(double);

/*!
  \slots This slot is used to set the lower limit of the QKnob scale.
*/
    void setMinValue(double);

/*!
  \slots This slot is used to set the upper limit of the QKnob scale.
*/
    void setMaxValue(double);



/*!
  \slots This slot is used to set the units in the QKnob widget.
*/
    void setUnits(QString);

/*!
  \slots This slot is used to set the widget foreground color.
*/
    void setForeground(QColor);
/*!
  \slots This slot is used to set the widget background color.
*/
    void setBackground(QColor);



    QString leftLabel() const { return m_leftLabel; }
    void setLeftLabel(QString leftLabel);

    QString rightLabel() const { return m_rightLabel; }
    void setRightLabel(QString rightLabel);



    int knobModeInt() const { return (int) m_knobMode; }
    void setKnobModeInt(int knobModeInt) { m_knobMode = (QKnobMode) knobModeInt; update(); }
    QKnobMode knobMode() const { return m_knobMode; }
    void setKnobMode(QKnobMode knobMode) { m_knobMode = knobMode; update(); }


protected:
        void paintEvent(QPaintEvent *event);
        void resizeEvent(QResizeEvent *event);
        void changeEvent(QEvent * event);
public:


        QString controllerDataValue() const { return _controllerDataValue; }
        void setControllerDataValue(QString controllerValueId) { _controllerDataValue = controllerValueId; }

        QString controllerDataInstantValue() const { return _controllerDataInstantValue; }
        void setControllerDataInstantValue(QString controllerDataInstantValue) { _controllerDataInstantValue = controllerDataInstantValue; }

        double stepValue() const { return m_stepValue; }
        void setStepValue(double stepValue) { m_stepValue = stepValue; }

        QColor editingColor() const { return m_editingColor; }
        void setEditingColor(QColor editingColor) { m_editingColor = editingColor; }

        double instantValue() const { return m_instantValue; }
        void setInstantValue(double instantValue);

        bool showValueSign() const { return _showValueSign; }
        void setShowValueSign(bool showValueSign) { _showValueSign = showValueSign; }





private:

    void thresholdManager();
    int digits(int val);  
    void recalculateData(double value, double instantValue);
    void drawCircleOutside(QPainter *painter);
    void drawBackground(QPainter *painter);
    void drawArea(QPainter *painter);
    void drawTriangle(QPainter *painter);
    void drawCircleInternal(QPainter *painter);
    void drawLabelBg(QPainter *painter);
    void drawLabel(QPainter *painter, QFont *textFont, QFont *extraTextFont);
    void drawText(QPainter & painter, qreal x, qreal y, Qt::Alignment flags, const QString & text, QRectF * boundingRect = 0);
    void drawUnits(QPainter *painter);
    void drawThresholdLine(QPainter *painter);
    void setClipping(QPainter *painter);

    int frameNo ;
    int delta;
    int alpha;
    int diameterCircleBig;
    int diameterCircleLittle;
    int knobOffset;
    int degree;
    QString text;
    QString instantText;

    double m_value;    
    double m_instantValue;
    double m_maxValue, m_minValue;    
    bool editingValue;

    QString m_label;
    QString m_units;
    QSize *m_labelSize;
    QColor m_foreground;
    QColor m_background;

    QFont *_textFont;
    QFont *_extraTextFont;


    QString _controllerDataValue;
    QString _controllerDataInstantValue;

    double m_stepValue;
    QString m_leftLabel;
    QString m_rightLabel;
    QString m_leftLabelDraw;
    QString m_rightLabelDraw;
    QColor m_editingColor;

    bool _showValueSign;

    QKnobMode m_knobMode;
};


#endif // QKnob_H

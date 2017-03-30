#ifndef QMIXERLABEL_H
#define QMIXERLABEL_H

#include "abstractwidget.h"

#include <QWidget>
#include <axelpainter.h>

class AxelLabel : public AbstractWidget
{
    Q_OBJECT
    Q_ENUMS(AxelLabelSeparators)
    Q_ENUMS(AxelLabelExtension)
    Q_ENUMS(AxelLavelStatus)

    Q_PROPERTY(QString textPrimary READ textPrimary WRITE setTextPrimary)
    Q_PROPERTY(QString textSecondary READ textSecondary WRITE setTextSecondary)
    Q_PROPERTY(QColor colorOn READ colorOn WRITE setColorOn)
    Q_PROPERTY(QColor colorOff READ colorOff WRITE setColorOff)
    Q_PROPERTY(QColor colorUndefined READ colorUndefined WRITE setColorUndefined)
    Q_PROPERTY(QColor fontColor READ fontColor WRITE setFontColor)
    Q_PROPERTY(QString imagePath READ imagePath WRITE setImagePath)
    Q_PROPERTY(QString image2Path READ image2Path WRITE setImage2Path)
    Q_PROPERTY(int imageIndex READ imageIndex WRITE setImageIndex)
    Q_PROPERTY(AxelLavelStatus status READ status WRITE setStatus)
    Q_PROPERTY(AxelLabelSeparators separators READ separators WRITE setSeparators)
    Q_PROPERTY(AxelLabelExtension extension READ extension WRITE setExtension)
    Q_PROPERTY(Qt::Alignment textAlignment READ textAlignment WRITE setTextAlignment)
    //Enum Property workaround
    Q_PROPERTY(int separatorsInt READ separatorsInt WRITE setSeparatorsInt)
    Q_PROPERTY(int extensionInt READ extensionInt WRITE setExtensionInt)
    Q_PROPERTY(int statusInt READ statusInt WRITE setStatusInt)

    //Controller variable id
    Q_PROPERTY(QString controllerDataText READ controllerDataText WRITE setControllerDataText)
    Q_PROPERTY(QString controllerDataStatus READ controllerDataStatus WRITE setControllerDataStatus)
    Q_PROPERTY(QString controllerDataSecondaryText READ controllerDataSecondaryText WRITE setControllerDataSecondaryText)
    Q_PROPERTY(QString controllerDataImageIndex READ controllerDataImageIndex WRITE setControllerDataImageIndex)


public:

    enum AxelLabelSeparators
    {
        AxelLabelSeparators_NONE = 0,
        AxelLabelSeparators_RIGHT = 1,
        AxelLabelSeparators_LEFT = 2,
        AxelLabelSeparators_BOTH = 3
    };

    enum AxelLabelExtension
    {
        AxelLabelExtension_NONE = 0,
        AxelLabelExtension_IMAGE = 1,
        AxelLabelExtension_TEXT = 2,
        AxelLabelExtension_IMAGE_TEXT = 3
    };

    enum AxelLavelStatus
    {
        AxelLavelStatus_OFF = 0,
        AxelLavelStatus_ON = 1,
        AxelLavelStatus_UNDEFINED = 2
    };



    //MixerLabel(QWidget *parent = 0, MixerLabelConfiguration *configuration = 0);
    AxelLabel(QWidget *parent = 0);    
    void resizeEvent(QResizeEvent * event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *e);
    bool ManageRemoteCommand(QString propertyName, QJsonValue command, bool init, QString * error) Q_DECL_OVERRIDE;
    void SetJsonData(QJsonObject* data) Q_DECL_OVERRIDE;
    QJsonObject* GetJsonData() Q_DECL_OVERRIDE;
    QJsonObject* GetJsonDataVars() Q_DECL_OVERRIDE;

    void setImage(QString path);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void updateFontSize(int height);

//    WidgetConfiguration *baseConf(){ return _baseConf;}
//    void setBaseConf(WidgetConfiguration *baseConf){ _baseConf = baseConf; }

    QString textPrimary(){ return _textPrimary;}
    void setTextPrimary(QString value){ _textPrimary = value; update(); }

    QString textSecondary(){ return _textSecondary;}
    void setTextSecondary(QString value);

    QColor colorOn(){ return _colorOn;}
    void setColorOn(QColor value){ _colorOn = value; update();}

    QColor colorOff(){ return _colorOff;}
    void setColorOff(QColor value)
    {
        _colorOff = value;
        update();
    }

    QColor colorUndefined() const { return _colorUndefined; }
    void setColorUndefined(QColor colorUndefined) { _colorUndefined = colorUndefined; }

    AxelLavelStatus status() { return _status; }
    void setStatus(AxelLavelStatus enabled);
    int statusInt(){ return (int)_status;}
    void setStatusInt(int value){ setStatus((AxelLavelStatus) value); }

    QColor fontColor(){ return _fontColor;}
    void setFontColor(QColor value){ _fontColor = value; update();}

    QString imagePath(){ return _imagePath;}
    void setImagePath(QString value){ _imagePath = value; setImage(_imagePath); update();}

    QString image2Path() const { return _image2Path; }
    void setImage2Path(QString image2Path) { _image2Path = image2Path; setTextSecondary(_textSecondary);}

    int imageIndex() const { return _imageIndex; }
    void setImageIndex(int imageIndex);

    AxelLabelSeparators separators(){ return _separators;}
    void setSeparators(AxelLabelSeparators value){ _separators = value; update();}
    int separatorsInt(){ return (int)_separators;}
    void setSeparatorsInt(int value){ _separators = (AxelLabelSeparators)value; update();}

    AxelLabelExtension extension(){ return _extentension;}
    void setExtension(AxelLabelExtension value){ _extentension = value; update();}
    int extensionInt(){ return (int)_extentension;}
    void setExtensionInt(int value){ _extentension = (AxelLabelExtension)value; this->resize(this->geometry().width(), this->geometry().height());}

    Qt::Alignment textAlignment() const { return _textAlignment; }
    void setTextAlignment(Qt::Alignment textAlignment){ _textAlignment = textAlignment; }


    QString controllerDataText() const { return _controllerDataText; }
    void setControllerDataText(QString value) { _controllerDataText = value;}

    QString controllerDataStatus() const { return _controllerDataStatus; }
    void setControllerDataStatus(QString value) { _controllerDataStatus = value;}

    QString controllerDataSecondaryText() const { return _controllerDataSecondaryText; }
    void setControllerDataSecondaryText(QString controllerDataSecondaryText) { _controllerDataSecondaryText = controllerDataSecondaryText; }

    QString controllerDataImageIndex() const { return _controllerDataImageIndex; }
    void setControllerDataImageIndex(QString controllerDataImageIndex) { _controllerDataImageIndex = controllerDataImageIndex; }



signals:

public slots:





private:
    void configure();
    void drawText(QPainter & painter, qreal x, qreal y, int flags, const QString & text, QRectF * boundingRect = 0, QFont * font = 0);
    //void FontCalculate(int size);

    AxelLavelStatus _status;
//    QString m_text_primary;
//    QString m_text_secondary;
//    QColor m_colorOn;
//    QColor m_colorOff;
//    QColor m_fontColor;
//    QImage m_image;

    QFont _fontText;
    QFont _fontTextBig;
    bool _textSecondaryOn;
    bool _textSecondaryOff;
    QString _fontFamily;
//    int _textWidthPrimary;
//    int _textWidthSecondary;

    //WidgetConfiguration *_baseConf;
    QString _textPrimary;
    QString _textSecondary;    
    QColor _colorOn;
    QColor _colorOff;
    QColor _fontColor;
    QString _imagePath;
    QString _image2Path;
    QString _currentImagePath;
    int _imageIndex;
    QImage *_resizedImage;
    AxelLabelSeparators _separators;
    AxelLabelExtension _extentension;
    Qt::Alignment _textAlignment;

    QString _controllerDataText;
    QString _controllerDataStatus;
    QString _controllerDataSecondaryText;
    QString _controllerDataImageIndex;

    long _paintWidth;
    long _paintHeight;

    bool _lock;



    QColor _colorUndefined;
};

#endif // QMIXERLABEL_H

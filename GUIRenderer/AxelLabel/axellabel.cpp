#include "axellabel.h"
#include "logger.h"

#include <QPainter>
#include <QFontDatabase>
#include <QJsonObject>
#include <QFileInfo>
#include <QResizeEvent>
#include "jsonserializer.h"


AxelLabel::AxelLabel(QWidget *parent)
    : AbstractWidget(parent)
{
    _paintWidth = 300;
    _paintHeight = 200;

    _resizedImage = 0;

    //Logger::Instance()->AddLog("start getting font color", LoggerLevel_DEBUGVERBOSE);
    if(parent)
        _fontColor = parent->palette().color(parent->backgroundRole());
    //Logger::Instance()->AddLog("end getting font color", LoggerLevel_DEBUGVERBOSE);

    _separators = AxelLabelSeparators_NONE;
    _extentension = AxelLabelExtension_NONE;
    _textPrimary = "Label";
    _textSecondary = "Text";
    _textSecondaryOn = false;
    _textSecondaryOff = false;
    _imageIndex = 0;

    _textAlignment = Qt::AlignCenter;

    _colorOn = QColor(Qt::red);
    _colorOff = QColor(Qt::white);
    _colorUndefined = QColor(Qt::yellow);
    _fontColor = QColor(Qt::black);

    //_baseConf = new WidgetConfiguration();
    //setGeometry(QRect(0, 0, 150, 50));

    configure();
}


void AxelLabel::configure()
{

    _status = AxelLavelStatus_OFF;

//    if(_baseConf != 0)
//        this->setGeometry(*_baseConf->position->pos);

    //Logger::Instance()->AddLog("Configuring AxelLabel 1", LoggerLevel_DEBUGVERBOSE);

    //int id = QFontDatabase::addApplicationFont(":/CALIBRIB.TTF");


//    Logger::Instance()->AddLog("Configuring AxelLabel 2");

//    if (QFontDatabase::applicationFontFamilies(id).count() <= 0)
//        return;
    //_fontFamily = QFontDatabase::applicationFontFamilies(id).at(0);
    //_fontFamily = "Calibri";
    _fontFamily = "Arial";


    //Logger::Instance()->AddLog("Configuring AxelLabel 3 fontFamily=" + _fontFamily, LoggerLevel_DEBUGVERBOSE);

    updateFontSize(this->height());    

    //Logger::Instance()->AddLog("Configuring AxelLabel 4", LoggerLevel_DEBUGVERBOSE);
}


void AxelLabel::setImage(QString value)
{
    if(!QFile::exists(value))
        return;

    if((_extentension == AxelLabelExtension_IMAGE_TEXT) && (_currentImagePath == value))
        return;

    _currentImagePath = value;

    QImage *image = new QImage(value);

//    if (value != 0)
//        _resizedImage = new QImage(_image->scaled(_paintWidth, _paintHeight - 85 - 5 , Qt::KeepAspectRatio, Qt::SmoothTransformation));
//    else
//        _resizedImage = new QImage();


    if (image != 0)
        _resizedImage = new QImage(image->scaled(this->width(), this->height() * 0.50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    else
        _resizedImage = new QImage();


    delete image;
}

void AxelLabel::setStatus(AxelLavelStatus status)
{
    _status = status;
    update();
}

void AxelLabel::paintEvent(QPaintEvent* event)
{
//    if(_lock)
//        return;
    _lock = true;

    Q_UNUSED(event);
    QPainter painter(this);
    AbstractWidget::PainterSetup(&painter);
//    AxelPainter painter(this);
//    painter.setMode(AxelPainter::pmVectorized);
//    painter.setMode(AxelPainter::pmNoCaching);
//    painter.setMode(AxelPainter::pmNonCosmetic, false);

    painter.setRenderHint(QPainter::Antialiasing, true);

    long width = _paintWidth;
    long height = _paintHeight;

    //Debug background    
    //painter.fillRect(0, 0, width, height, Qt::blue);


    QColor brushColor;
    if(_status == AxelLavelStatus_ON)
        brushColor = _colorOn;
    else if(_status == AxelLavelStatus_UNDEFINED)
        brushColor = _colorUndefined;
    else
        brushColor = _colorOff;

    QRect primaryTextArea;

    if(_extentension == AxelLabelExtension_NONE)
        primaryTextArea = QRect(0, 0, width, height);
    else
        primaryTextArea = QRect(width * 0.10, height * 0.05, width * 0.80, height * 0.40);

    QRectF primaryFloatTextArea(primaryTextArea);
    QRectF secondaryFloatTextArea(primaryFloatTextArea.x(), height - (height * 0.075), primaryFloatTextArea.width() * 1.2, primaryFloatTextArea.height());

    painter.fillRect(primaryTextArea, brushColor);

    painter.save();

    int textX = width / 2; // - (_textWidthPrimary / 2);

    painter.setFont(_fontText);

    QColor penColor = _fontColor;
    painter.setPen(QPen(penColor));
    //QPainterPath textPath;

    //Logger::Instance()->AddLog("Drawing text start objectName=" + objectName() + " fontFamily=" + _fontFamily + "...");



    if(_extentension == AxelLabelExtension_NONE)
    {
        //textPath.addText(textX , height * 0.54, _fontText, _textPrimary);
        //drawText(painter, textX, height * 0.54 /*+ _fontText.pointSize()/2*/, Qt::AlignVCenter | Qt::AlignHCenter, _textPrimary);
        long h = height * 0.5;
        //drawText(painter, textX, h /*+ _fontText.pointSize()/2*/, Qt::AlignCenter /*Qt::AlignHCenter | Qt::AlignBottom*/, _textPrimary);
        if(_textAlignment == Qt::AlignCenter)
            drawText(painter, textX, h , _textAlignment, _textPrimary, &primaryFloatTextArea, &_fontText);
        else if(_textAlignment & Qt::AlignLeft)
            drawText(painter, 0, h, _textAlignment, _textPrimary, &primaryFloatTextArea, &_fontText);
        else if(_textAlignment & Qt::AlignRight)
            drawText(painter, width, h, _textAlignment, _textPrimary, &primaryFloatTextArea, &_fontText);
    }
    else
    {
        //textPath.addText(textX , height  * 0.395 , _fontText, _textPrimary);
        //drawText(painter, textX, height  * 0.29 , Qt::AlignCenter, _textPrimary);

        int y = primaryTextArea.y() + primaryTextArea.height() * 1;
        drawText(painter, textX, y , Qt::AlignHCenter | Qt::AlignBottom, _textPrimary, &primaryFloatTextArea, &_fontText);
        //painter.drawText(primaryTextArea, _textPrimary, Qt::AlignHCenter | Qt::AlignVCenter);
    }

    //painter.fillPath(textPath, QBrush(_fontColor));

    //draw secondary item
    if ((_extentension == AxelLabelExtension_IMAGE) && (_resizedImage != 0))
    {
        try{
            painter.drawImage(width / 2 - (_resizedImage->width() / 2 ), height - (_resizedImage->height()), *_resizedImage);
        }
        catch(...){
        }
    }
    else if(_extentension == AxelLabelExtension_TEXT)
    {
//        QPainterPath textPathSecondary;
//        textPathSecondary.addText(width / 2 - (_textWidthSecondary / 2) , height - (height * 0.075)  , _fontText, _textSecondary);
//        painter.fillPath(textPathSecondary, QBrush(_colorOff));

        painter.setPen(QPen(_colorOff));
        drawText(painter, textX, height * 0.925, Qt::AlignBottom | Qt::AlignHCenter, _textSecondary, &secondaryFloatTextArea, &_fontText);
    }
    else if (_extentension == AxelLabelExtension_IMAGE_TEXT)
    {
        if(_resizedImage != 0)
        {
            try{
                painter.drawImage(width * 0.30 - (_resizedImage->width() / 2 ), height - (_resizedImage->height()), *_resizedImage);
            }
            catch(...){
            }
        }

        painter.setFont(_fontTextBig);
        if(_textSecondaryOn)
            painter.setPen(QPen(_colorOn));
        else
            painter.setPen(QPen(_colorOff));

        secondaryFloatTextArea = QRect(0,0, width * 0.5, height * 0.2);
        drawText(painter, width * 0.75, height, Qt::AlignBottom | Qt::AlignHCenter, _textSecondary, &secondaryFloatTextArea, &_fontTextBig);
    }



    painter.restore();

    if ((_separators == AxelLabel::AxelLabelSeparators_LEFT) || (_separators == AxelLabel::AxelLabelSeparators_BOTH))
    {
        painter.fillRect(0, 0, 3, height, _colorOff);
    }
    if ((_separators == AxelLabel::AxelLabelSeparators_RIGHT) || (_separators == AxelLabel::AxelLabelSeparators_BOTH))
    {
        painter.fillRect(width - 3, 0, 3, height, _colorOff);
    }

    _lock = false;
}


/*
 * When you pass a starting point for drawing text, you're effectively drawing text on an a large rectangle
 * that has the bottom-left corner at the given point. So all you need is to offer a suitable "infinite"
 * rectangle based on your starting point and the chosen alignment
 * http://stackoverflow.com/questions/24831484/how-to-align-qpainter-drawtext-around-a-point-not-a-rectangle
 */
void AxelLabel::drawText(QPainter & painter, qreal x, qreal y, int flags,
              const QString & text, QRectF * boundingRect, QFont * font)
{
    if(boundingRect == 0)
        return;
    if(font == 0)
        return;

    double fontStretch = 1;
    painter.resetTransform();

    if(boundingRect != 0)
    {
        QFontMetrics fm(*font);
        int maxTextWidth = boundingRect->width() - 10;
        int textWidth = fm.width(text);
        if(textWidth > maxTextWidth)
        {
            fontStretch = (double)maxTextWidth / textWidth;
            painter.scale(fontStretch,1);
        }
     }

    const qreal size = 32767.0;
    QPointF corner(x / fontStretch, y - size);
    if (flags & Qt::AlignHCenter) corner.rx() -= size/2.0;
    else if (flags & Qt::AlignRight) corner.rx() -= size;
    if (flags & Qt::AlignVCenter) corner.ry() += size/2.0;
    else if (flags & Qt::AlignTop) corner.ry() += size;
    else flags |= Qt::AlignBottom;
    QRectF rect(corner, QSizeF(size, size));

    painter.drawText(rect, flags, text, boundingRect);
}



//PAINTING USING VIEWPORT
/*
 *
void AxelLabel::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    //Debug background
    painter.setBrush(Qt::blue);
    painter.drawRect(0, 0, width(), height());

    //Disegna sempre in un area quadrata di lato uguale al minimo tra altezza e larghezza visibili, senza di questo stretcha
//    int side = qMin(width(), height());
//    painter.setViewport((width() - side) / 2, (height() - side) / 2,side, side);

    painter.setWindow(0,0, _paintWidth, _paintHeight);

    //Debug background
    painter.setBrush(Qt::blue);
    painter.drawRect(0, 0, _paintWidth, _paintHeight);

    if(_enabled)
        painter.setBrush(QBrush(_colorOn));
    else
        painter.setBrush(QBrush(_colorOff));

    if(_extentension == AxelLabelExtension_NONE)
        painter.drawRect(0, 0, _paintWidth, _paintHeight);
    else
        painter.drawRect(52, 26, 200, 58);


    painter.save();
    //painter.resetTransform();
    //draw text
    QPainterPath textPath;
    if(_extentension == AxelLabelExtension_NONE)
        textPath.addText(150 - (_textWidthPrimary / 2) , _paintHeight / 2 + _fontText.pointSize()/2 , _fontText, _textPrimary);
    else
        textPath.addText(150 - (_textWidthPrimary / 2) , 79 , _fontText, _textPrimary);

    painter.fillPath(textPath, QBrush(_fontColor));
    //painter.restore();

    //draw secondary item
    if ((_extentension == AxelLabelExtension_IMAGE) && (_image != 0))
        painter.drawImage(_paintWidth / 2 - (_image->width() / 2 ), _paintHeight - (_image->height()), *_image);
    else if(_extentension == AxelLabelExtension_TEXT)
    {
        QPainterPath textPathSecondary;
        textPathSecondary.addText(_paintWidth / 2 - (_textWidthSecondary / 2) , _paintHeight - 15  , _fontText, _textSecondary);
        painter.fillPath(textPathSecondary, QBrush(_colorOff));
    }

    painter.restore();

    painter.setBrush(_colorOff);
    if ((_separators == AxelLabel::AxelLabelSeparators_LEFT) || (_separators == AxelLabel::AxelLabelSeparators_BOTH))
    {
        painter.drawRect(0, 0, 5, _paintHeight);
    }
    if ((_separators == AxelLabel::AxelLabelSeparators_RIGHT) || (_separators == AxelLabel::AxelLabelSeparators_BOTH))
    {
        painter.drawRect(_paintWidth - 5, 0, 5, _paintHeight);
    }

}

*/


void AxelLabel::resizeEvent(QResizeEvent * event)
{
    AbstractWidget::resizeEvent(event);
    _paintWidth = event->size().width();
    _paintHeight = event->size().height();

    if((_paintHeight <= 0) || (_paintHeight > 50000))
        _paintHeight = 1;
    if((_paintWidth <= 0) || (_paintWidth > 50000))
        _paintWidth = 1;

    updateFontSize(event->size().height());    


//    QFontMetrics fm(_fontText);
//    _textWidthPrimary = fm.width(_textPrimary);
//    _textWidthSecondary = fm.width(_textSecondary);

    setImage(_currentImagePath);
    setTextSecondary(_textSecondary);
}


void AxelLabel::updateFontSize(int height)
{
    //    if(_extentension == AxelLabelExtension_NONE)
    //        _fontText = QFont(fontFamily, 150);
    //    else
    //        _fontText = QFont(fontFamily, 45);

    int newSize = height;

    if(_extentension == AxelLabelExtension_NONE)
        newSize = height * 0.70;
    else
        newSize = height * 0.22;

    _fontText = QFont(_fontFamily, newSize);
    _fontTextBig = QFont(_fontFamily, newSize * 1.75);


    //_fontText.setStyleStrategy(QFont::ForceOutline);
//    _fontText.setKerning(true);
#ifdef ON_MIXER
    _fontText.setLetterSpacing(QFont::PercentageSpacing, 90);
    _fontText.setBold(true);
    _fontTextBig.setLetterSpacing(QFont::PercentageSpacing, 90);
    _fontTextBig.setBold(true);
#else
    _fontText.setBold(true);
    _fontTextBig.setBold(true);
#endif

//    QFontMetrics fm(_fontText);
//    Logger::Instance()->AddLog("font width= " + QString::number(fm.averageCharWidth()));
}

void AxelLabel::setTextSecondary(QString value)
{
    bool parseOk = false;
    int valueN = value.toInt(&parseOk);
    if(parseOk)
    {
        if (valueN == 0)
            setImage(_image2Path);
        else
        {
            if(_imageIndex > 0)
                setImage(_image2Path);
            else
                setImage(_imagePath);
        }


        if(valueN <= 80)
            _textSecondaryOn = false;
        else
            _textSecondaryOn = true;

        if(valueN > 0)
            _textSecondaryOff = false;
        else
            _textSecondaryOff = true;

    }

    _textSecondary = value; update();
}


void AxelLabel::setImageIndex(int imageIndex)
{
    _imageIndex = imageIndex;

    if(_textSecondaryOff)
        return;

    if(imageIndex > 0)
        setImage(_image2Path);
    else
        setImage(_imagePath);

    update();
}


void AxelLabel::mousePressEvent(QMouseEvent *)
{
    //qDebug() << "mouseclick";

}


void AxelLabel::mouseReleaseEvent(QMouseEvent *)
{

}

bool AxelLabel::ManageRemoteCommand(QString propertyName, QJsonValue value, bool init, QString * error)
{    
    Q_UNUSED(error);

    bool retVal = AbstractWidget::ManageRemoteCommand(propertyName, value, init, error);

    if (propertyName == "textPrimary")
    {
        if (value.isString())
            setTextPrimary(value.toString());
    }
    else if (propertyName == "textSecondary")
    {        
        if (value.isString())
            setTextSecondary(value.toString());
    }
    else if (propertyName == "imageIndex")
    {
        int index = value.toInt(0);
        setImageIndex(index);
    }
    else if (propertyName == "status")
    {
        if (!this->isEnabled())
            return retVal;
        if(!value.isDouble())
            return retVal;

        int setVal = value.toDouble(0);

        if(setVal == 1)
            setStatus(AxelLavelStatus_ON);
        else if(setVal == 0)
            setStatus(AxelLavelStatus_OFF);
        else if(setVal == 2)
            setStatus(AxelLavelStatus_UNDEFINED);
        else
        {
            retVal = false;
            error = new QString("Error setting AxelLabel " + this->objectName() + " property=" + propertyName + " value=" + value.toString() );
        }
    }
    return retVal;
}


void AxelLabel::SetJsonData(QJsonObject *data)
{
    if (data == 0)
        return;
    AbstractWidget::SetJsonData(data);

    _textPrimary = (*data)["textPrimary"].toString();
    _textSecondary = (*data)["textSecondary"].toString();
    _imagePath = (*data)["image"].toString();
    _image2Path = (*data)["image2"].toString();
    _currentImagePath = _imagePath;

    QString on = (*data)["colorOn"].toString();
    QString off = (*data)["colorOff"].toString();
    QString undef = (*data)["colorUndefined"].toString();
    _colorOn = GetColorFromString(on);
    _colorOff = GetColorFromString(off);
    _colorUndefined = GetColorFromString(undef);
    _fontColor = GetColorFromString((*data)["fontColor"].toString());

    _separators = (AxelLabelSeparators)(*data)["separators"].toInt();
    _extentension = (AxelLabelExtension)(*data)["extension"].toInt();
    _textAlignment = (Qt::Alignment)(*data)["textAlignment"].toInt();
}


QJsonObject* AxelLabel::GetJsonData()
{
    QJsonObject* retObj = AbstractWidget::GetJsonData();


    retObj->insert("textPrimary", QJsonValue(_textPrimary));
    retObj->insert("textSecondary", QJsonValue(_textSecondary));
    retObj->insert("image", QJsonValue(_imagePath));
    retObj->insert("image2", QJsonValue(_image2Path));

    retObj->insert("colorOn", QJsonValue(GetStringFromColor(_colorOn)));
    retObj->insert("colorOff", QJsonValue(GetStringFromColor(_colorOff)));
    retObj->insert("colorUndefined", QJsonValue(GetStringFromColor(_colorUndefined)));
    retObj->insert("fontColor", QJsonValue(GetStringFromColor(_fontColor)));

    retObj->insert("separators", QJsonValue((int)_separators));
    retObj->insert("extension", QJsonValue((int)_extentension));    
    retObj->insert("textAlignment", QJsonValue((int)_textAlignment));

    return retObj;
}


QJsonObject* AxelLabel::GetJsonDataVars()
{
    QJsonObject* retObj = AbstractWidget::GetJsonDataVars();
    QJsonArray properties;
    if(retObj->contains("properties"))
        properties = (*retObj)["properties"].toArray();
    else
        properties = QJsonArray();

    QJsonObject value = JsonSerializer::GetVarsProperty(_controllerDataStatus, "INTEGER", "status");
    if(!value.isEmpty())
        properties.append(value);

    value = JsonSerializer::GetVarsProperty(_controllerDataText, "STRING", "textPrimary");
    if(!value.isEmpty())
        properties.append(value);

    value = JsonSerializer::GetVarsProperty(_controllerDataSecondaryText, "STRING", "textSecondary");
    if(!value.isEmpty())
        properties.append(value);

    value = JsonSerializer::GetVarsProperty(_controllerDataImageIndex, "INTEGER", "imageIndex");
    if(!value.isEmpty())
        properties.append(value);

    if(properties.isEmpty())
        return 0;


    retObj->insert("properties", properties);
    return retObj;
}





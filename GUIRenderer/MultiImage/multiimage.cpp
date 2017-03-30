#include "multiimage.h"

#include <QPainter>
#include <QFontDatabase>
#include <QJsonObject>
#include <QFileInfo>
#include <QJsonArray>
#include <logger.h>
#include <QMovie>
#include "jsonserializer.h"


MultiImage::MultiImage(QWidget *parent)
    : AbstractWidget(parent)
{
    _paintWidth = 480;
    _paintHeight = 480;
    _showImageIndex = 0;
    _imageLabel = 0;

    //_imageList = QStringList();
    _imageObjList = new QList<QImage*>();
    QStringList items = QStringList();
    setImageList(items);
    configure();
}


void MultiImage::configure()
{
    update();
}

MultiImage::~MultiImage()
{
    DisposeImageList();
}


void MultiImage::DisposeImageList()
{
    if (_imageObjList != 0)
    {
        for(int i = 0; i< _imageObjList->count(); i++)
            delete _imageObjList->at(i);
        _imageObjList->clear();        
    }
    if(_imageLabel != 0)
    {
        delete _imageLabel;
        _imageLabel = 0;
    }
}

//setImageList resizing images to the size of the widget (paint without setWindow)
void MultiImage::setImageList(QStringList &value)
{
    DisposeImageList();
    _imageList = value;

    //Get the max side of the image list
    QImage *imgBuff = NULL;
    foreach (QString i, _imageList)
    {
        Logger::Instance()->AddLog("QImage: starting load...", LoggerLevel_DEBUGVERBOSE, "QIMAGE_START_LOAD");
        if(QFile::exists(i))
            imgBuff = new QImage(i);
        else
            imgBuff = new QImage(":/multiImage.png");

        Logger::Instance()->AddLog("QImage: done load", LoggerLevel_DEBUGVERBOSE, "QIMAGE_END_LOAD");

        int h = imgBuff->height();
        int w = imgBuff->width();
        double ar = (double)w / h;

        h = height();
        w = h * ar;

        if(w > width())
        {
            w = width();
            h = w / ar;
        }


        Logger::Instance()->AddLog("QImage: starting resize...", LoggerLevel_DEBUGVERBOSE, "QIMAGE_START_RESIZE");

        QImage *imgScaled = new QImage(imgBuff->scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        Logger::Instance()->AddLog("QImage: done resize", LoggerLevel_DEBUGVERBOSE, "QIMAGE_END_RESIZE");

        delete imgBuff;
        imgBuff = NULL;
        _imageObjList->append(imgScaled);
    }

    update();
}


//setImageList resizing images to the bigger one of the list (paint with setWindow)
/*
void MultiImage::setImageList(QStringList &value)
{
    DisposeImageList();
    _imageList = value;
    QList <QImage*> imgBuffList;
    int sideMax = (_paintWidth > _paintHeight ? _paintWidth : _paintHeight);

    //Get the max side of the image list
    QImage *imgBuff;
    foreach (QString i, _imageList)
    {        
        imgBuff = NULL;
        if(QFile::exists(i))
        {
            imgBuff = new QImage(i);
            int h = imgBuff->height();
            int w = imgBuff->width();
            if(h > sideMax)
                sideMax = h;
            if(w > sideMax)
                sideMax = w;
        }
        imgBuffList.append(imgBuff);
    }
    imgBuff = NULL;

    _paintWidth = sideMax;
    _paintHeight = sideMax;


    //Load image
    foreach(QImage* iBuff, imgBuffList)
    {
        QImage *img = NULL;

        if(iBuff == NULL)
        {
            QImage *imgDefault = new QImage(":/multiImage.png");
            img = new QImage(imgDefault->scaled(_paintWidth, _paintHeight , Qt::KeepAspectRatio, Qt::SmoothTransformation));
            delete imgDefault;
            imgDefault = NULL;
        }
        else
        {
            img = new QImage(iBuff->scaled(_paintWidth, _paintHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }

        _imageObjList->append(img);


        delete iBuff;
        img = NULL;
    }

    imgBuffList.clear();

    update();
}
*/


//setImageList fixed size
/*
void MultiImage::setImageList(QStringList &value)
{
    bool loadDefaultImage = true;

    DisposeImageList();
    _imageList = value;

    foreach (QString i, _imageList)
    {
        QImage *img = NULL;
        if(QFile::exists(i))
        {
            QImage *imgBuff = new QImage(i);

            img = new QImage(imgBuff->scaled(_paintWidth, _paintHeight , Qt::KeepAspectRatio, Qt::SmoothTransformation));

            _imageObjList->append(img);
            loadDefaultImage = false;

            delete imgBuff;
            imgBuff = NULL;
        }


        if (loadDefaultImage)
        {
            QImage *imgBuff = new QImage(":/multiImage.png");
            QImage *img = new QImage(imgBuff->scaled(_paintWidth, _paintHeight , Qt::KeepAspectRatio, Qt::SmoothTransformation));
            _imageObjList->append(img);
            delete imgBuff;
            imgBuff = NULL;
        }

        img = NULL;
    }


    update();
}
*/


void MultiImage::setShowImageIndex(int value)
{
    _showImageIndex = value;
    if(_imageLabel != 0)
    {
        delete _imageLabel;
        _imageLabel = 0;
    }

    //Crappy workaround until I find a better way to load gifs
    if((_showImageIndex < _imageList.count()) && (_showImageIndex < _imageObjList->count()))
    {
        QString imagePath = _imageList.at(_showImageIndex);
        QImage* imageObj = _imageObjList->at(_showImageIndex);
        if((QFile::exists(imagePath)) && imagePath.toLower().endsWith(".gif"))
        {
            _imageLabel = new  QLabel(this);
            QMovie *movie = new QMovie(imagePath);
            movie->setParent(_imageLabel);
            _imageLabel->setStyleSheet("background-color: rgba(255, 255, 255, 0);");
            _imageLabel->setGeometry(width() / 2 - imageObj->width() / 2, height() / 2 - imageObj->height() / 2, imageObj->width(), imageObj->height());
            movie->setScaledSize(QSize(_imageLabel->width(), _imageLabel->height()));
            _imageLabel->setMovie(movie);
            movie->start();
        }
    }

    update();
}


void MultiImage::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    AbstractWidget::PainterSetup(&painter);

    painter.setRenderHint(QPainter::Antialiasing, true);

    //painter.setWindow(0,0, _paintWidth, _paintHeight);

    //Debug background
//    painter.setBrush(Qt::blue);
//    painter.drawRect(0,0,width(),height());

    if(_imageLabel != 0)
        return;

    QImage* showImage = NULL;
    if (_showImageIndex < _imageObjList->count())
        showImage = _imageObjList->at(_showImageIndex);
    else if(_imageObjList->count() > 0)
        showImage = _imageObjList->at(0);

    Logger::Instance()->AddLog("QImage: starting draw...", LoggerLevel_DEBUGVERBOSE, "QIMAGE_START_DRAW");

    if (showImage != 0)
        painter.drawImage(width() / 2 - showImage->width() / 2, height() / 2 - showImage->height() / 2 , *showImage);

    Logger::Instance()->AddLog("QImage: done draw", LoggerLevel_DEBUGVERBOSE, "QIMAGE_END_DRAW");


    showImage = NULL;
}


void MultiImage::resizeEvent(QResizeEvent* event)
{
    AbstractWidget::resizeEvent(event);
    if(_imageLabel != 0)
        setShowImageIndex(_showImageIndex);

}

void MultiImage::mouseReleaseEvent(QMouseEvent *)
{

}

void MultiImage::mousePressEvent(QMouseEvent *)
{

}

bool MultiImage::ManageRemoteCommand(QString propertyName, QJsonValue value, bool init, QString * error)
{
    Q_UNUSED(error);

    bool retVal = AbstractWidget::ManageRemoteCommand(propertyName, value, init, error);

    int imageIdx = value.toInt(0);

    if(!retVal)
        error = new QString("Error setting MultiImage index " + this->objectName() + " property=" + propertyName + " value=" + value.toString() );
    else
        _showImageIndex = imageIdx;


    return retVal;
}


void MultiImage::SetJsonData(QJsonObject *data)
{
    if (data == 0)
        return;
    AbstractWidget::SetJsonData(data);

    QStringList items = QStringList();
    if((*data)["images"].isArray())
    {
        QJsonArray values = (*data)["images"].toArray();
        foreach (QJsonValue v, values) {
            items.append(v.toString());
        }
    }

    setImageList(items);
    setShowImageIndex((*data)["showIndex"].toInt());

}


QJsonObject* MultiImage::GetJsonData()
{
    QJsonObject* retObj = AbstractWidget::GetJsonData();

    QJsonArray images = QJsonArray();
    foreach (QString img, _imageList) {
        images.append(QJsonValue(img));
    }
    retObj->insert("images", QJsonValue(images));
    retObj->insert("showIndex",QJsonValue(_showImageIndex));


    return retObj;
}

QJsonObject *MultiImage::GetJsonDataVars()
{
    QJsonObject* retObj = AbstractWidget::GetJsonDataVars();

    QJsonArray properties;
    if(retObj->contains("properties"))
        properties = (*retObj)["properties"].toArray();
    else
        properties = QJsonArray();

    QJsonObject value = JsonSerializer::GetVarsProperty(_controllerDataValue, "INTEGER", "value");
    if(!value.isEmpty())
        properties.append(value);

    if(properties.isEmpty())
        return 0;

    retObj->insert("properties", properties);
    return retObj;
}



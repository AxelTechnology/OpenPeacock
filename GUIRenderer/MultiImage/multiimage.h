#ifndef QMULTIIMAGE_H
#define QMULTIIMAGE_H

#include "abstractwidget.h"

#include <QWidget>
#include <QScopedPointer>
#include <QLabel>

class MultiImage : public AbstractWidget
{
    Q_OBJECT
    Q_PROPERTY(int showImageIndex READ showImageIndex WRITE setShowImageIndex)
    Q_PROPERTY(QStringList imageList READ imageList WRITE setImageList)
    Q_PROPERTY(QString controllerDataValue READ controllerDataValue WRITE setControllerDataValue)


public:

    MultiImage(QWidget *parent = 0);
    ~MultiImage();

    void DisposeImageList();
    void configure();
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *e);
    bool ManageRemoteCommand(QString propertyName, QJsonValue value, bool init, QString * error) Q_DECL_OVERRIDE;
    void SetJsonData(QJsonObject* data) Q_DECL_OVERRIDE;
    QJsonObject* GetJsonData() Q_DECL_OVERRIDE;
    QJsonObject* GetJsonDataVars() Q_DECL_OVERRIDE;
    void SetValue(bool enabled);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);

    QStringList imageList(){ return _imageList; }
    void setImageList(QStringList &value);

    int showImageIndex(){ return _showImageIndex;}
    void setShowImageIndex(int value);


    QString controllerDataValue() const
    {
        return _controllerDataValue;
    }
    void setControllerDataValue(QString controllerValueId)
    {
        _controllerDataValue = controllerValueId;
    }

signals:

public slots:



private:

    QStringList _imageList;
    QList<QImage*> *_imageObjList;
    QLabel* _imageLabel;
    int _showImageIndex;
    int _paintWidth;
    int _paintHeight;

    QString _controllerDataValue;
};

#endif // QMULTIIMAGE_H

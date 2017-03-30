#ifndef QMETADATA_H
#define QMETADATA_H

#include "abstractwidget.h"

#include <QWidget>
#include <QImage>

class Metadata : public AbstractWidget
{
    Q_OBJECT
    Q_PROPERTY(QString controllerDataValue READ controllerDataValue WRITE setControllerDataValue)
    Q_PROPERTY(int bind READ bind WRITE setBind)


public:

    Metadata(QWidget *parent = 0);
    ~Metadata();

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
    QSize sizeHint() const;


    QString controllerDataValue() const
    {
        return _controllerDataValue;
    }
    void setControllerDataValue(QString controllerValueId)
    {
        _controllerDataValue = controllerValueId;
    }

    int bind() const { return _bind; }
    void setBind(int bind) { _bind = bind; }

signals:

public slots:





private:

    QString _controllerDataValue;
    QImage _iconImage;
    int _paintWidth;
    int _paintHeight;
    int _bind;
};

#endif // QMETADATA_H

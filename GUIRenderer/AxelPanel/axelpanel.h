#ifndef QAXELPANEL_H
#define QAXELPANEL_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include "abstractwidget.h"

class AxelPanel : public AbstractWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor colorBg READ colorBg WRITE setColorBg)

public:

    AxelPanel(QWidget *parent = 0);
    virtual ~AxelPanel();
    void paintEvent(QPaintEvent *e);
    bool ManageRemoteCommand(QString propertyName, QJsonValue value, bool init, QString * error) Q_DECL_OVERRIDE;
    void SetJsonData(QJsonObject* data) Q_DECL_OVERRIDE;
    QJsonObject* GetJsonData() Q_DECL_OVERRIDE;
    QJsonObject* GetJsonDataVars() Q_DECL_OVERRIDE;

    QColor colorBg(){ return _colorBg;}
    void setColorBg(QColor value){ _colorBg = value; update();}

private slots:
    void Blink();

private:

    QColor _colorBg;
    QColor _colorBg_bk;
    QColor _colorBlack;
    int _paintWidth;
    int _paintHeight;    

    QTimer *timer;
    QElapsedTimer elapsedTimer;
    bool isBlack;


};

#endif // QAXELPANEL_H

#include "axelmessagebox.h"
#include "logger.h"

#include <QPainter>
#include <QFontDatabase>
#include <QJsonObject>
#include <QFileInfo>
#include <QResizeEvent>
#include <axellabel.h>
#include <multiimage.h>



AxelMessagebox::AxelMessagebox(QWidget *parent)
    : AbstractWidget(parent)
{
    _parent = parent;
}


AxelMessagebox::~AxelMessagebox()
{
    if (_pnlMessagebox != 0)
        delete _pnlMessagebox;
}

void AxelMessagebox::ShowMessage(QString text, MessageboxType type, int timeoutMsec)
{

    _pnlMessagebox = new AxelPanel(this);
    _pnlMessagebox->setGeometry(20, 100, 760, 70);
    _pnlMessagebox->setColorBg(QColor(30,30,30));
    _pnlMessagebox->show();

    AxelLabel* _lblTitle = new AxelLabel(_pnlMessagebox);
    _lblTitle->setGeometry(5, 5, 750, 16);
    _lblTitle->setColorOff(QColor(0,0,0,0));
    _lblTitle->setFontColor(QColor(254,254,0));
    if(type == MessageboxType_ERROR)
        _lblTitle->setTextPrimary("ERROR");
    else if(type == MessageboxType_WARNING)
        _lblTitle->setTextPrimary("WARNING");
    else if(type == MessageboxType_WAIT)
        _lblTitle->setTextPrimary("PLEASE WAIT");
    else
        _lblTitle->setTextPrimary("INFO");
    _lblTitle->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _lblTitle->show();

    MultiImage* image = new MultiImage(_pnlMessagebox);
    image->setGeometry(6,20,42,42);
    QStringList iconList;
    iconList << ":/info.png" << ":/error.png" << ":/warning.png" << ":/loading.gif";
    image->setImageList(iconList);
    image->setShowImageIndex((int) type);
    image->show();


    AxelLabel* _lblMessage = new AxelLabel(_pnlMessagebox);
    _lblMessage->setGeometry(55,20, 700, 42);
    _lblMessage->setColorOff(QColor(50,50,50));
    _lblMessage->setFontColor(QColor(254,254,254));
    _lblMessage->setTextPrimary(text);
    _lblMessage->show();

    if(timeoutMsec == 0)
        this->setAutoHideMsec(3000);
    else
        this->setAutoHideMsec(timeoutMsec);


    this->show();
}



void AxelMessagebox::paintEvent(QPaintEvent* event)
{

    Q_UNUSED(event);
    QPainter painter(this);
    AbstractWidget::PainterSetup(&painter);

    painter.setRenderHint(QPainter::Antialiasing, true);

}

bool AxelMessagebox::ManageRemoteCommand(QString propertyName, QJsonValue command, bool init, QString * error)
{
    return AbstractWidget::ManageRemoteCommand(propertyName, command, init, error);
}

void AxelMessagebox::SetJsonData(QJsonObject *data)
{
    if (data == 0)
        return;
    AbstractWidget::SetJsonData(data);

}


QJsonObject* AxelMessagebox::GetJsonData()
{
    QJsonObject* retObj = AbstractWidget::GetJsonData();
    return retObj;
}


QJsonObject* AxelMessagebox::GetJsonDataVars()
{
    QJsonObject* retObj = AbstractWidget::GetJsonDataVars();
    return retObj;
}





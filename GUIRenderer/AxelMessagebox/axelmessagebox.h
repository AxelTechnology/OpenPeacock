#ifndef QMIXERMESSAGEBOX_H
#define QMIXERMESSAGEBOX_H

#include "abstractwidget.h"

#include <QWidget>
#include <axelpanel.h>

class AxelMessagebox : public AbstractWidget
{
    Q_OBJECT
    //Q_ENUMS(AxelLabelSeparators)


public:

    enum MessageboxType
    {
        MessageboxType_INFO = 0,
        MessageboxType_ERROR = 1,
        MessageboxType_WARNING = 2,
        MessageboxType_WAIT = 3
    };

    void ShowMessage(QString text, MessageboxType type, int timeoutMsec);

    //MixerLabel(QWidget *parent = 0, MixerLabelConfiguration *configuration = 0);
    AxelMessagebox(QWidget *parent = 0);
    ~AxelMessagebox();
    void paintEvent(QPaintEvent *e);
    bool ManageRemoteCommand(QString propertyName, QJsonValue command, bool init, QString * error) Q_DECL_OVERRIDE;
    void SetJsonData(QJsonObject* data) Q_DECL_OVERRIDE;
    QJsonObject* GetJsonData() Q_DECL_OVERRIDE;
    QJsonObject* GetJsonDataVars() Q_DECL_OVERRIDE;


signals:

public slots:


private:

    QWidget *_parent;
    AxelPanel *_pnlMessagebox;

};

#endif // QMIXERMESSAGEBOX_H

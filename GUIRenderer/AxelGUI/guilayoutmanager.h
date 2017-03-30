#ifndef GUILAYOUT_H
#define GUILAYOUT_H

#include <QWidget>
#include <QList>
#include <udpsocket.h>
#include <networkmanager.h>
#include "jsonserializer.h"
#include "axellabel.h"
#include "axelmessagebox.h"
#include "timesynchronizer.h"
#include "ilayoutmanager.h"

class GuiLayoutManager : public QObject, public ILayoutManager
{
    Q_OBJECT
    Q_INTERFACES(ILayoutManager)

    //Q_PROPERTY(QList<QWidget *> widgetList READ widgetList)
    UdpSocket *_socket;
    QList<QWidget *> *_widgetList;
    bool _layoutShowing;
    QWidget *_parent;

    qulonglong _layoutLoadCounter;
    QString  _lastLoadedLayout;
    QString  _lastLoadedLayoutJsonData;



public:
    GuiLayoutManager(QWidget *parent = 0);
    bool Initialize(int listenPort, bool setDemoLayout);
    QList<QWidget *> * widgetList() const;
    bool ShowLayout();
    bool ClearLayout();
    QString GetLayout();
    static void PrintUsage();

    QString GetVersion();
    QString GetLastLayoutId();
    QString GetLastLayoutJsonData();
    qulonglong GetLayoutCounter();
    void ResetDebugInfo();

signals:

public slots:
    void setData(QString data);
    void NetworkManagerStatusUpdate(SystemManagerEnums::NetworkConfigurationStatus status);

private :
    void SetLayout(GUIData* data);        
    void SetupTestLayout();
    void SetupSplashLayout();
    void SetWidgetDataJson(QString id, QJsonObject obj);
    void SetWidgetData(QString id, QString property, QJsonValue value, bool init);
    void setDataPrivate(QString data);
    void setDataObject(QJsonObject obj);
    void ShowMessagebox(QString message, AxelMessagebox::MessageboxType type, int timeoutMsec);
    void HideMessagebox();

    AxelMessagebox* _messagebox;

    JsonSerializer* _serializer;

    TimeSynchronizer* _timeSynchronizer;    
    QThread* _timeSynchronizerThread;
    QString _timeSynchronizerSettingsFilePath;

    NetworkManager* _networkManager;
    QThread* _networkManagerThread;

    static const QString _version;

};

#endif // GUILAYOUT_H

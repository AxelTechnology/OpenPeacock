#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <systemmanagerenums.h>

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    NetworkManager(QObject *parent = 0);
    ~NetworkManager();

    void SetJsonObj(QJsonObject jsonObj);

signals:
    void ConfigStatusUpdate(SystemManagerEnums::NetworkConfigurationStatus status);

public slots:

private:
    bool LoadNetworkConf();
    bool SaveNetworkConf();
    bool ApplyNetworkConf();

    SystemManagerEnums::NetworkMode _networkMode1;
    SystemManagerEnums::NetworkMode _networkMode2;
    QString _ipAddress1;
    QString _ipAddress2;
    QString _netmask1;
    QString _netmask2;
    QString _gateway;

};

#endif // NETWORKMANAGER_H

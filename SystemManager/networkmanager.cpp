#include "networkmanager.h"
#include "logger.h"

#include <QFile>
#include <QJsonObject>
#include <QProcess>
#include <QTextStream>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent)
{    
    _networkMode1 = SystemManagerEnums::NetworkMode_STATIC;
    _networkMode2 = SystemManagerEnums::NetworkMode_STATIC;
    _ipAddress1 = QString();
    _ipAddress2 = QString();
    _netmask1 = QString();
    _netmask2 = QString();
    _gateway = QString();

    LoadNetworkConf();
}


NetworkManager::~NetworkManager(){ }


void NetworkManager::SetJsonObj(QJsonObject jsonObj)
{
    if(jsonObj.isEmpty())
        return;

    emit ConfigStatusUpdate(SystemManagerEnums::NetworkConfigurationStatus_CONFIGURING);

    LoadNetworkConf();

    if(jsonObj.contains("mode1"))
        _networkMode1 = (SystemManagerEnums::NetworkMode)jsonObj["mode1"].toInt();
    if(jsonObj.contains("mode2"))
        _networkMode1 = (SystemManagerEnums::NetworkMode)jsonObj["mode2"].toInt();
    if(jsonObj.contains("ipaddress1"))
        _ipAddress1 = jsonObj["ipaddress1"].toString();
    if(jsonObj.contains("ipaddress2"))
        _ipAddress2 = jsonObj["ipaddress2"].toString();
    if(jsonObj.contains("netmask1"))
        _netmask1 = jsonObj["netmask1"].toString();
    if(jsonObj.contains("netmask2"))
        _netmask2 = jsonObj["netmask2"].toString();
    if(jsonObj.contains("gateway"))
        _gateway = jsonObj["gateway"].toString();


    if(SaveNetworkConf())
        emit ConfigStatusUpdate(SystemManagerEnums::NetworkConfigurationStatus_SUCCESS);
    else
        emit ConfigStatusUpdate(SystemManagerEnums::NetworkConfigurationStatus_FAIL);
}


bool NetworkManager::LoadNetworkConf()
{
    QString fileName("/etc/network/interfaces");
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        Logger::Instance()->AddLog("Error opening " + fileName, LoggerLevel_ERROR, "NET_GET");
        return false;
    }
    QTextStream stream(&file);

    bool eth0 = false;
    bool eth1 = false;

    while(!stream.atEnd())
    {
        QString line = stream.readLine().trimmed();
        if(line.isEmpty())
            continue;

        if(line.contains("eth0:1"))
        {
            eth1 = true;
            eth0 = false;
        }
        else if(line.contains("eth0"))
        {
            eth0 = true;
            eth1 = false;
        }

        QString address;
        if(line.contains("address") || line.contains("netmask") || line.contains("gateway"))
        {
            QStringList splittedLine = line.split(" ");
            if(splittedLine.count() < 2)
                continue;
            address = splittedLine.at(1);
            if(address.isEmpty())
                continue;
        }


        if(line.contains("dhcp"))
        {
            if(eth0)
            {
                _networkMode1 = SystemManagerEnums::NetworkMode_DHCP;
                _ipAddress1.clear();
                _netmask1.clear();
                _gateway.clear();
            }
            else if(eth1)
            {
                _networkMode2 = SystemManagerEnums::NetworkMode_DHCP;
                _ipAddress2.clear();
                _netmask2.clear();
            }
        }
        if(line.contains("static"))
        {
            if(eth0)
                _networkMode1 = SystemManagerEnums::NetworkMode_STATIC;
            else if(eth1)
                _networkMode2 = SystemManagerEnums::NetworkMode_STATIC;
        }
        else if(line.contains("address"))
        {
            if(eth0)
                _ipAddress1 = address;
            else if(eth1)
                _ipAddress2 = address;
        }
        else if(line.contains("netmask"))
        {
            if(eth0)
                _netmask1 = address;
            else if(eth1)
                _netmask2 = address;
        }
        else if(line.contains("gateway"))
        {
            if(eth0)
                _gateway = address;
        }

    }

    file.close();
    return true;
}


bool NetworkManager::SaveNetworkConf()
{
    Logger::Instance()->AddLog("Setting network configuration", LoggerLevel_INFO);

    QString netConf("auto lo\n"
                    "iface lo inet loopback\n"
                    "\n");
    if(_networkMode1 == SystemManagerEnums::NetworkMode_DHCP)
        netConf += QString("auto eth0\n"
                        "iface eth0 inet dhcp\n"
                        "\n");
    else if(_networkMode1 == SystemManagerEnums::NetworkMode_STATIC)
    {
        netConf += QString("auto eth0\n"
                        "iface eth0 inet static\n");
        if(!_ipAddress1.isEmpty())
            netConf += QString("address " + _ipAddress1 + "\n");
        if(!_netmask1.isEmpty())
            netConf += QString("netmask " + _netmask1 + "\n");
        if(!_gateway.isEmpty())
            netConf += QString("gateway " + _gateway + "\n");
         netConf += ("\n");

    }
    if(_networkMode2 == SystemManagerEnums::NetworkMode_DHCP)
        netConf += QString("auto eth0:1\n"
                        "iface eth0 inet dhcp\n"
                        "\n");
    else if(_networkMode2 == SystemManagerEnums::NetworkMode_STATIC)
    {
        netConf += QString("auto eth0:1\n"
                        "iface eth0:1 inet static\n");
        if(!_ipAddress2.isEmpty())
            netConf += QString("address " + _ipAddress2 + "\n");
        if(!_netmask2.isEmpty())
            netConf += QString("netmask " + _netmask2 + "\n");
        netConf += ("\n");
    }


    QString fileName("/etc/network/interfaces");
    QFile file(fileName);
    if (!file.exists())
        return false;

    if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        Logger::Instance()->AddLog("Error opening " + fileName, LoggerLevel_ERROR, "NET_SET");
        return false;
    }
    QTextStream stream(&file);

    Logger::Instance()->AddLog("Writing network conf:" + netConf, LoggerLevel_INFO, "NET_SET_STRING");

    stream << netConf;
    file.close();


    return ApplyNetworkConf();
}


bool NetworkManager::ApplyNetworkConf()
{
    QStringList args;
    args << "restart";
    QProcess p;
    p.start("/etc/init.d/S40network", args) ;
    p.waitForFinished();
    if(p.exitCode() < 0)
    {
        Logger::Instance()->AddLog("NTP service script error", LoggerLevel_ERROR, "IFUP_STARTUP");
        return false;
    }

    return true;
}

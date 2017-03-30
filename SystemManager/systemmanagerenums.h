#ifndef SYNCHRONIZERENUMS_H
#define SYNCHRONIZERENUMS_H

#include <QObject>


class SystemManagerEnums : public QObject
{
    Q_OBJECT

public:
    enum TimeSyncMode
    {        
        TimeSyncMode_RTC = 0,
        TimeSyncMode_NTP = 1
    };

    enum NetworkMode
    {
        NetworkMode_STATIC = 0,
        NetworkMode_DHCP = 1
    };

    enum NetworkConfigurationStatus
    {
        NetworkConfigurationStatus_CONFIGURING = 0,
        NetworkConfigurationStatus_SUCCESS = 1,
        NetworkConfigurationStatus_FAIL = 2
    };
};


#endif // SYNCHRONIZERENUMS_H

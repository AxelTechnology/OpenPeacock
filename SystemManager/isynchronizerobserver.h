#ifndef ISYNCHRONIZEROBSERVER_H
#define ISYNCHRONIZEROBSERVER_H

#include <QDateTime>
#include <systemmanagerenums.h>


class ISynchronizerObserver
{    
    Q_ENUMS(SystemManagerEnums::TimeSyncMode)

    public:
        virtual bool SetTime(QDateTime dateTime) = 0;
        virtual bool SetTimeZone(int timeZone) = 0;
        virtual bool SetNtpAddress(QString ntpAddress) = 0;
        virtual void setSyncMode(SystemManagerEnums::TimeSyncMode syncMode) = 0;
};


#endif // ISYNCHRONIZEROBSERVER_H

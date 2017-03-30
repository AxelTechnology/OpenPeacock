#ifndef TIMESYNCHRONIZER_H
#define TIMESYNCHRONIZER_H

#include <QObject>
#include <QTimer>
#include <timesynchronizersettings.h>
#include <isynchronizerobserver.h>
#include <systemmanagerenums.h>
#include <qdatetime.h>
#include <logger.h>


class TimeSynchronizer: public QObject, public ISynchronizerObserver
{
    Q_OBJECT

public:

    Q_PROPERTY(SystemManagerEnums::TimeSyncMode syncMode READ syncMode WRITE setSyncMode)
    Q_PROPERTY(TimeSynchronizerSettings* settings READ settings)

    TimeSynchronizer(QObject *parent = 0);
    ~TimeSynchronizer();

    bool SetTime(QDateTime dateTime);
    bool SetTimeZone(int timeZone);
    bool SetNtpAddress(QString ntpAddress);

    SystemManagerEnums::TimeSyncMode syncMode() const { return m_syncMode; }
    void setSyncMode(SystemManagerEnums::TimeSyncMode syncMode);
    bool StartSync(QString settingsFile);

    TimeSynchronizerSettings* settings() const
    {
        return _settings;
    }

public slots:


private slots:
    void updateTimerTick();


private:

    bool SetSysTime(QDateTime dateTime);
    QDateTime GetRTCTime();
    void SetRTCTime(QDateTime rtcTime);
    int LaunchRTCProcess(QString hexParam, bool write);
    int LaunchRTCProcess(QString hexParam, bool write, QString hexValue);
    bool SetNtpService(bool start);
    TimeSynchronizerSettings * _settings;

    QTimer _tmrTimeUpdate;
    QDateTime _lastSyncTime;

    bool _initializing;

    void LoadTimezones(QString timezonesFilepath);
    QStringList _timezones;

    SystemManagerEnums::TimeSyncMode m_syncMode;

    static const int _RTCUpdateIntervalSec;
    static const QString _RTCProcessPath;

};

#endif // TIMESYNCHRONIZER_H

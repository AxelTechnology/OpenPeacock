#ifndef TIMESYNCHRONIZERSETTINGS_H
#define TIMESYNCHRONIZERSETTINGS_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QStringList>
#include <QTimer>
#include <isynchronizerobserver.h>
#include <systemmanagerenums.h>


class TimeSynchronizerSettings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool settingsRead READ settingsRead)

    Q_PROPERTY(int timeZoneIdx READ timeZoneIdx WRITE setTimeZoneIdx)
    Q_PROPERTY(QString ntpAddress READ ntpAddress WRITE setNtpAddress)
    Q_PROPERTY(SystemManagerEnums::TimeSyncMode syncMode READ syncMode WRITE setSyncMode)

    Q_PROPERTY(QString settingsFile READ settingsFile WRITE setSettingsFile)

public:

    TimeSynchronizerSettings(QObject *parent = 0);
    ~TimeSynchronizerSettings();
    bool Start(ISynchronizerObserver * _observer, QString settingFile);
    bool Stop();

    bool LoadSettingsFile();
    bool SaveSettingsFile();

    QJsonObject GetJsonObj();
    void SetJsonObj(QJsonObject jsonObj);

    bool settingsRead() const { return _settingsRead; }

    int timeZoneIdx() const { return _timeZoneIdx; }
    void setTimeZoneIdx(int timeZoneIdx) { _timeZoneIdx = timeZoneIdx; }

    QString ntpAddress() const { return _ntpAddress; }
    void setNtpAddress(QString ntpAddress) { _ntpAddress = ntpAddress; }

    SystemManagerEnums::TimeSyncMode syncMode() const { return _syncMode; }
    void setSyncMode(SystemManagerEnums::TimeSyncMode syncMode) { _syncMode = syncMode; }

    QString settingsFile() const { return _settingsFilePath; }
    void setSettingsFile(QString value) { _settingsFilePath = value; }


private slots:
    void updateTimerTick();

private:
    void ApplySettings();

    QTimer _tmrTimeUpdate;
    ISynchronizerObserver * _observer;
    QString _settingsFilePath;
    QDateTime _lastSettingsFileRead;


    bool _settingsRead;

    int _timeZoneIdx;
    QString _ntpAddress;
    SystemManagerEnums::TimeSyncMode _syncMode;    


};

#endif // TIMESYNCHRONIZERSETTINGS_H

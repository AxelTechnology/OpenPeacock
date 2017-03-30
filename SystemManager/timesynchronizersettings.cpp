#include "timesynchronizersettings.h"
#include <QFileInfo>
#include <QTextStream>
#include "logger.h"

TimeSynchronizerSettings::TimeSynchronizerSettings(QObject *parent)
    : QObject(parent)
{
    _timeZoneIdx = 0;

    _ntpAddress = QString();
    _syncMode = SystemManagerEnums::TimeSyncMode_NTP;
    _settingsRead = false;
    _observer = 0;
    _lastSettingsFileRead.setMSecsSinceEpoch(0);
    connect(&_tmrTimeUpdate, SIGNAL(timeout()),this, SLOT(updateTimerTick()));
}


TimeSynchronizerSettings::~TimeSynchronizerSettings()
{
    _observer = 0;
    if(_tmrTimeUpdate.isActive())
        _tmrTimeUpdate.stop();
}


bool TimeSynchronizerSettings::Start(ISynchronizerObserver *observer, QString settingFile)
{
    if(_tmrTimeUpdate.isActive())
        Logger::Instance()->AddLog("TimeSynchronizerSettings already started", LoggerLevel_WARNING);

    this->_observer = observer;
    _settingsFilePath = settingFile;

    _tmrTimeUpdate.start(1000);
    Logger::Instance()->AddLog("TimeSynchronizerSettings started", LoggerLevel_INFO);

    return true;
}

bool TimeSynchronizerSettings::Stop()
{
    if(_tmrTimeUpdate.isActive())
        _tmrTimeUpdate.stop();

    Logger::Instance()->AddLog("TimeSynchronizerSettings stopped", LoggerLevel_INFO);

    return true;
}


bool TimeSynchronizerSettings::LoadSettingsFile()
{
    QFile settingsFile(_settingsFilePath);
    if (!settingsFile.open(QFile::ReadOnly | QFile::Text))
        return false;

    QTextStream in(&settingsFile);
    QString settingsContent = in.readAll();
    settingsFile.close();

    QJsonDocument doc = QJsonDocument::fromJson(settingsContent.toUtf8());
    if (!doc.isObject())
    {
        Logger::Instance()->AddLog("Error reading time synchronizer settings: " + QFileInfo(settingsFile).absoluteFilePath() + "\n" + settingsContent, LoggerLevel_ERROR, "TIMESYNC_LOAD");
        return false;
    }

    SetJsonObj(doc.object());

    _settingsRead = true;

    return true;
}

bool TimeSynchronizerSettings::SaveSettingsFile()
{
    QJsonDocument doc;
    doc.setObject(GetJsonObj());
    QString jsonContent = doc.toJson();

    QFile file(_settingsFilePath);
    if(file.exists())
        file.remove();
    if(!file.open(QIODevice::ReadWrite))
    {
        Logger::Instance()->AddLog("Error opening " + _settingsFilePath, LoggerLevel_ERROR, "TIMESYNC_SAVE");
        return false;
    }

    QTextStream stream(&file);
    stream << jsonContent;
    file.close();

    return true;
}


void TimeSynchronizerSettings::updateTimerTick()
{
    QFileInfo fi(_settingsFilePath);

    if (!fi.exists(_settingsFilePath))
        return;

    if(fi.lastModified().toUTC() <= _lastSettingsFileRead)
        return;
    _lastSettingsFileRead = fi.lastModified().toUTC();

    if (!LoadSettingsFile())
        return;

    ApplySettings();
}



void TimeSynchronizerSettings::ApplySettings()
{
    if(_observer == 0)
        return;

    _observer->SetNtpAddress(_ntpAddress);
    _observer->SetTimeZone(_timeZoneIdx);
    _observer->setSyncMode(_syncMode);
}



QJsonObject TimeSynchronizerSettings::GetJsonObj()
{
    QJsonObject retObj;

    retObj.insert("timezone", QJsonValue(_timeZoneIdx));
    retObj.insert("ntpAddress", QJsonValue(_ntpAddress));
    retObj.insert("syncMode", QJsonValue((int)_syncMode));

    return retObj;
}

void TimeSynchronizerSettings::SetJsonObj(QJsonObject jsonObj)
{
    if(jsonObj.isEmpty())
        return;

    if(jsonObj.contains("timezone"))
        _timeZoneIdx = jsonObj["timezone"].toInt();
    if(jsonObj.contains("ntpAddress"))
        _ntpAddress = jsonObj["ntpAddress"].toString();
    if(jsonObj.contains("syncMode"))
        _syncMode = (SystemManagerEnums::TimeSyncMode)jsonObj["syncMode"].toInt();

    QString commandType = jsonObj["type"].toString();

    bool save = false;
    if(commandType == "timezone")
    {
        _timeZoneIdx = jsonObj["value"].toInt();
        save = true;
    }
    if(commandType == "ntpAddress")
    {
        _ntpAddress = jsonObj["value"].toString();
        save = true;
    }
    if(commandType == "DATE_NTP")
    {
        _syncMode = (SystemManagerEnums::TimeSyncMode)jsonObj["value"].toInt();
        save = true;
    }

    QDateTime curDateTime = QDateTime::currentDateTime();
    int year = curDateTime.date().year();
    int month = curDateTime.date().month();
    int day = curDateTime.date().day();
    int hours = curDateTime.time().hour();
    int minutes = curDateTime.time().minute();
    int seconds = curDateTime.time().second();

    if(commandType == "DATE_YEAR")
        year = jsonObj["value"].toInt() + 2000;

    if(commandType == "DATE_MONTH")
        month = jsonObj["value"].toInt() + 1;

    if(commandType == "DATE_DAY")
        day = jsonObj["value"].toInt() + 1;

    if(commandType == "DATE_HOUR")
        hours = jsonObj["value"].toInt();

    if(commandType == "DATE_MIN")
        minutes = jsonObj["value"].toInt();

    if(commandType == "DATE_SEC")
        seconds = jsonObj["value"].toInt();

    QDate date(year, month, day);
    QTime time(hours, minutes, seconds, 0);
    QDateTime dtBuff = QDateTime(date, time);
    if(_observer != 0)
        _observer->SetTime(dtBuff);

    if (save)
        SaveSettingsFile();

}


/*
void TimeSynchronizerSettings::SetJsonObj(QJsonObject jsonObj)
{
    if(jsonObj.isEmpty())
        return;

    if(jsonObj.contains("timezone"))
        _timeZoneIdx = jsonObj["timezone"].toInt();
    if(jsonObj.contains("ntpAddress"))
        _ntpAddress = jsonObj["ntpAddress"].toString();
    if(jsonObj.contains("syncMode"))
        _syncMode = (SystemManagerEnums::TimeSyncMode)jsonObj["syncMode"].toInt();


    QDateTime curDateTime = QDateTime::currentDateTime();
    int year = curDateTime.date().year();
    int month = curDateTime.date().month();
    int day = curDateTime.date().day();
    int hours = curDateTime.time().hour();
    int minutes = curDateTime.time().minute();
    int seconds = curDateTime.time().second();

    if(jsonObj.contains("dateTimeYear"))
        year = jsonObj["dateTimeYear"].toInt();

    if(jsonObj.contains("dateTimeMonth"))
        month = jsonObj["dateTimeMonth"].toInt();

    if(jsonObj.contains("dateTimeDay"))
        day = jsonObj["dateTimeDay"].toInt();

    if(jsonObj.contains("dateTimeHours"))
        hours = jsonObj["dateTimeHours"].toInt();

    if(jsonObj.contains("dateTimeMinutes"))
        minutes = jsonObj["dateTimeMinutes"].toInt();

    if(jsonObj.contains("dateTimeSeconds"))
        seconds = jsonObj["dateTimeSeconds"].toInt();

    QDate date(year + 2000, month, day);
    QTime time(hours, minutes, seconds, 0);
    QDateTime dtBuff = QDateTime(date, time);
    if(_observer != 0)
        _observer->SetSysTime(dtBuff);

}

*/

#include "timesynchronizer.h"

#include <QFile>
#include <QJsonArray>
#include <QProcess>
#include <QTextStream>


const int TimeSynchronizer::_RTCUpdateIntervalSec = 3600;
const QString TimeSynchronizer::_RTCProcessPath = "/root/adauO3K/adau_fw_i2c";


TimeSynchronizer::TimeSynchronizer(QObject *parent)
    : QObject(parent)
{
    _initializing = true;
    _settings = new TimeSynchronizerSettings(this);
    _lastSyncTime.setMSecsSinceEpoch(1);
    connect(&_tmrTimeUpdate, SIGNAL(timeout()),this, SLOT(updateTimerTick()));
    _tmrTimeUpdate.start(1000);

    Logger::Instance()->AddLog("TimeSynchronizer started", LoggerLevel_INFO);
}

TimeSynchronizer::~TimeSynchronizer()
{
    _tmrTimeUpdate.stop();
}

bool TimeSynchronizer::StartSync(QString settingsFile)
{
    if(_settings == 0)
        return false;

    LoadTimezones(":/timezones_linux.json");

    return _settings->Start(this, settingsFile);
}


void TimeSynchronizer::updateTimerTick()
{
    if(!_settings->settingsRead())
        return;

    QDateTime curDateTime = QDateTime::currentDateTime();

    if((qAbs(_lastSyncTime.secsTo(curDateTime)) < _RTCUpdateIntervalSec) && (!_initializing))
        return;


    QDateTime time;    
    if((m_syncMode == SystemManagerEnums::TimeSyncMode_RTC) || _initializing)
    {
        time = GetRTCTime();
        if(time.isValid())
            SetTime(time.toLocalTime());
        else
            Logger::Instance()->AddLog("Error getting RTC time", LoggerLevel_ERROR, "RTC_TIMEGET");
    }
    else if(m_syncMode == SystemManagerEnums::TimeSyncMode_NTP)
    {
        time = QDateTime::currentDateTimeUtc();
        SetRTCTime(time);
    }

    _initializing = false;

    _lastSyncTime = curDateTime;
}


bool TimeSynchronizer::SetTime(QDateTime dateTime)
{
    if (!SetSysTime(dateTime))
        return false;
    SetRTCTime(dateTime.toUTC());

}

bool TimeSynchronizer::SetSysTime(QDateTime dateTime)
{
    Logger::Instance()->AddLog("Setting system time with value " + dateTime.toString("yyyy-MM-dd hh:mm:ss"), LoggerLevel_INFO);

    QProcess p;
    QStringList args;
    args << "--set" << "" + dateTime.toString("yyyy-MM-dd hh:mm:ss") + "";

    p.start("date", args);
    p.waitForFinished();
    int retVal = p.exitCode();

    if(retVal < 0)
    {
        Logger::Instance()->AddLog("Error setting system time", LoggerLevel_ERROR, "RTC_TIMESET");
        return false;
    }

    return true;
}


bool TimeSynchronizer::SetTimeZone(int timeZone)
{
    if(timeZone >= _timezones.count())
    {
        Logger::Instance()->AddLog("Unknown timezone", LoggerLevel_ERROR, "TZ_UNKNOWN");
        return false;
    }

    Logger::Instance()->AddLog("Setting timezone: " + _timezones.at(timeZone), LoggerLevel_INFO);

    QProcess p;
    QStringList args;
    args << "/etc/localtime";

    p.start("unlink", args);
    p.waitForFinished();
    int retVal = p.exitCode();

    if(retVal < 0)
    {
        Logger::Instance()->AddLog("Error setting timezone", LoggerLevel_ERROR, "TZ_SET1");
        return false;
    }

    args.clear();
    args << "-s" << "/usr/share/zoneinfo/" + _timezones.at(timeZone) << "/etc/localtime";

    p.start("ln", args);
    p.waitForFinished();
    retVal = p.exitCode();

    if(retVal < 0)
    {
        Logger::Instance()->AddLog("Error setting timezone", LoggerLevel_ERROR, "TZ_SET2");
        return false;
    }



    return true;
}


bool TimeSynchronizer::SetNtpAddress(QString ntpAddress)
{

    Logger::Instance()->AddLog("Setting NTP address " + ntpAddress);

    QString ntpConf("#server 0.pool.ntp.org iburst\n"
                    "#server 1.pool.ntp.org iburst\n"
                    "server " + ntpAddress + "\n" +
                    "# Allow only time queries, at a limited rate, sending KoD when in excess.\n"
                    "# Allow all local queries (IPv4, IPv6)\n"
                    "restrict default nomodify nopeer noquery limited kod\n"
                    "restrict 127.0.0.1\n"
                    "restrict [::1]\n");

    QString fileName("/etc/ntp.conf");
    QFile file(fileName);
    if(!file.open(QIODevice::ReadWrite))
    {
        Logger::Instance()->AddLog("Error opening " + fileName, LoggerLevel_ERROR, "NTP_SET");
        return false;
    }
    QTextStream stream(&file);
    stream << ntpConf;
    file.close();
    return true;
}

void TimeSynchronizer::setSyncMode(SystemManagerEnums::TimeSyncMode syncMode)
{
    m_syncMode = syncMode;


    if(m_syncMode == SystemManagerEnums::TimeSyncMode_NTP)
    {
        Logger::Instance()->AddLog("Setting sync mode to NTP");
        SetNtpService(true);
    }
    else if(m_syncMode == SystemManagerEnums::TimeSyncMode_RTC)
    {
        Logger::Instance()->AddLog("Setting sync mode to RTC");
        SetNtpService(false);
    }
}


QDateTime TimeSynchronizer::GetRTCTime()
{
    QDateTime retVal = QDateTime::currentDateTime();

    if(!QFile::exists(_RTCProcessPath))
    {
        Logger::Instance()->AddLog("Cannot find RTC executable " + _RTCProcessPath, LoggerLevel_ERROR, "RTC_EXE");
        return retVal;
    }

    int year = LaunchRTCProcess("0x1D", false);
    int month = LaunchRTCProcess("0x1E", false);
    int day = LaunchRTCProcess("0x24", false);
    int hour = LaunchRTCProcess("0x20", false);
    int minute = LaunchRTCProcess("0x21", false);
    int second = LaunchRTCProcess("0x22", false);

    if((year < 0) || (month < 0) || (day < 0) || (hour < 0) || (minute < 0) || (second < 0))
    {
        Logger::Instance()->AddLog("Error getting RTC time", LoggerLevel_ERROR, "RTC_SET");
        return retVal;
    }

    QDate date(year + 2000, month, day);
    QTime time(hour, minute, second, 0);
    QDateTime dtBuff = QDateTime(date, time, Qt::UTC);

    Logger::Instance()->AddLog("Got RTC time: " + dtBuff.toString("yyyy-MM-dd hh:mm:ss.zzz"), LoggerLevel_INFO);

    return dtBuff;
}




void TimeSynchronizer::SetRTCTime(QDateTime rtcTime)
{
     Logger::Instance()->AddLog("Setting RTC time with value " + rtcTime.toString("yyyy-MM-dd hh:mm:ss.zzz"), LoggerLevel_INFO);

     if(!QFile::exists(_RTCProcessPath))
     {
         Logger::Instance()->AddLog("Cannot find RTC executable " + _RTCProcessPath, LoggerLevel_ERROR, "RTC_EXE");
         return;
     }


     int y = rtcTime.date().year() - 2000;     if(y < 0)
         y = 0;

     QString year = QString("0x%1").arg(y, 2, 16, QLatin1Char( '0' ));
     QString month = QString("0x%1").arg(rtcTime.date().month(), 2, 16, QLatin1Char( '0' ));
     QString day = QString("0x%1").arg(rtcTime.date().day(), 2, 16, QLatin1Char( '0' ));
     QString hour = QString("0x%1").arg(rtcTime.time().hour(), 2, 16, QLatin1Char( '0' ));
     QString minute = QString("0x%1").arg(rtcTime.time().minute(), 2, 16, QLatin1Char( '0' ));
     QString second = QString("0x%1").arg(rtcTime.time().second(), 2, 16, QLatin1Char( '0' ));
     LaunchRTCProcess("0x1D", true, year);
     LaunchRTCProcess("0x1E", true, month);
     LaunchRTCProcess("0x24", true, day);
     LaunchRTCProcess("0x20", true, hour);
     LaunchRTCProcess("0x21", true, minute);
     LaunchRTCProcess("0x22", true, second);
}

int TimeSynchronizer::LaunchRTCProcess(QString hexParam, bool write)
{
    return LaunchRTCProcess(hexParam, write, "0x00");
}

int TimeSynchronizer::LaunchRTCProcess(QString hexParam, bool write, QString hexValue)
{
    QString rwFlag = "0";
    if (write)
        rwFlag = "1";

    QString paramBuff = hexParam;
    QString valBuf = hexValue;

    QProcess p;
    QStringList args;    
    args << "P" << "q" << "0x0E" << paramBuff << valBuf << rwFlag;

    p.start(_RTCProcessPath, args);
    p.waitForFinished();
    int procVal = p.exitCode();

    if(procVal < 0)
        return -1;
    if(write)
        return 0;

    bool ok;

    QString output = p.readAllStandardOutput();

    if(output.isEmpty())
        return -1;

    int retval = output.toInt(&ok);

    if(!ok)
        return -1;

    return retval;
}

bool TimeSynchronizer::SetNtpService(bool start)
{
    QStringList args;

    if(start)
    {
        Logger::Instance()->AddLog("Starting NTP Service");
        args << "restart";
    }
    else
    {
        Logger::Instance()->AddLog("Stopping NTP Service");
        args << "stop";
    }

    QProcess p;
    p.start("/etc/init.d/S49ntp", args) ;
    p.waitForFinished();
    if(p.exitCode() < 0)
    {
        Logger::Instance()->AddLog("NTP service script error", LoggerLevel_ERROR, "NTP_STARTUP");
        return false;
    }

    return true;
}


void TimeSynchronizer::LoadTimezones(QString timezonesFilepath)
{
    if (!QFile::exists(timezonesFilepath))
    {
        Logger::Instance()->AddLog("Error loading timezones file: " + timezonesFilepath, LoggerLevel_ERROR, "TZ_FILE");
        return;
    }

    QFile tzFile(timezonesFilepath);
    if (!tzFile.open(QFile::ReadOnly | QFile::Text))
    {
        Logger::Instance()->AddLog("Error loading timezones file: " + timezonesFilepath, LoggerLevel_ERROR, "TZ_FILE");
        return ;
    }

    QTextStream in(&tzFile);
    QString tzContent = in.readAll();
    tzFile.close();

    QJsonDocument doc = QJsonDocument::fromJson(tzContent.toUtf8());
    if (!doc.isArray())
    {
        Logger::Instance()->AddLog("Error loading timezones file: " + timezonesFilepath, LoggerLevel_ERROR, "TZ_FILE");
        return;
    }

    _timezones.clear();

    QJsonArray tzArray = doc.array();
    foreach (QJsonValue tz, tzArray) {
        if (!tz.isString())
            continue;
        _timezones.append(tz.toString());
    }

}

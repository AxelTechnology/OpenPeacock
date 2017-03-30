#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QDateTime>
#include <QMetaEnum>
#include <QMap>


enum LoggerLevel
{
    LoggerLevel_INFO = 0,
    LoggerLevel_WARNING = 1,
    LoggerLevel_ERROR = 2,
    LoggerLevel_DEBUG = 3,
    LoggerLevel_DEBUGVERBOSE = 4
};
//Q_ENUM(LoggerLevel)

class Logger : public QObject
{    
    Q_OBJECT

public:

    Q_PROPERTY(LoggerLevel maxLogLevel READ maxLogLevel WRITE setMaxLogLevel)
    Q_PROPERTY(bool logHysteresisEnabled READ logHysteresisEnabled WRITE setLogHysteresisEnabled)
    Q_PROPERTY(long logHysteresisValue READ logHysteresisValue WRITE setLogHysteresisValue)

    Logger();
    static Logger *Instance();

    void AddLog(QString logLine, LoggerLevel logLevel = LoggerLevel_INFO);
    void AddLog(QString logLine, LoggerLevel logLevel, QString logId);

    LoggerLevel maxLogLevel() const { return m_maxLogLevel; }
    void setMaxLogLevel(LoggerLevel maxLogLevel) { m_maxLogLevel = maxLogLevel; }


    bool logHysteresisEnabled() const { return m_logHysteresisEnabled; }
    void setLogHysteresisEnabled(bool logErrorsHysteresis) { m_logHysteresisEnabled = logErrorsHysteresis; }

    void ClearLogHysteresisHistory();

    long logHysteresisValue() const { return m_logHysteresisValue; }
    void setLogHysteresisValue(long logHysteresisValue) { m_logHysteresisValue = logHysteresisValue; }

    void AddLogfilter(QString filter);
    void ClearLogFilter();



private:
    QDateTime _lastLogTime;
    LoggerLevel m_maxLogLevel;
    bool m_logHysteresisEnabled;
    long m_logHysteresisValue;
    QMap<QString, QDateTime> m_errorMap;
    QStringList m_logFilter;
};

#endif // LOGGER_H

#include "logger.h"
#include <QDebug>
#include <QMetaEnum>

#include "singleton.h"
#define LoggerInstance Singleton<Logger>::instance()

Logger::Logger()
{
    _lastLogTime = QDateTime::currentDateTime();
    m_maxLogLevel = LoggerLevel_ERROR;
    m_logHysteresisValue = 10000;
    m_logHysteresisEnabled = false;
    m_logFilter.clear();
}


void Logger::AddLog(QString logLine, LoggerLevel logLevel)
{
    AddLog(logLine, logLevel, QString());
}

void Logger::AddLog(QString logLine, LoggerLevel logLevel, QString logId)
{
    if (logLevel > m_maxLogLevel)
        return;

    QDateTime dateTime = QDateTime::currentDateTime();

    //Filter
    if((!m_logFilter.isEmpty()) && (logLevel >= LoggerLevel_DEBUG))
    {
        bool found = false;
        foreach (QString f, m_logFilter)
        {
           if (logLine.contains(f))
           {
               found = true;
               break;
           }
        }
        if(!found)
            return;
    }

    //Hysteresis
    if (m_logHysteresisEnabled && (!logId.isEmpty()))
    {
        if(m_errorMap.contains(logId))
        {
            long elapsedMsec = dateTime.currentDateTime().toMSecsSinceEpoch() - m_errorMap[logId].toMSecsSinceEpoch();
            if(elapsedMsec < m_logHysteresisValue)
                return;
            m_errorMap[logId] = dateTime;
        }
        else
            m_errorMap.insert(logId, dateTime);

    }

    long elapsedMsec = dateTime.toMSecsSinceEpoch() - _lastLogTime.toMSecsSinceEpoch();
    _lastLogTime = dateTime;

    QString line = dateTime.toString("yyyy-MM-dd hh:mm:ss.zzz") + " level=" + QString::number(logLevel) + " tmsec=" + QString::number(elapsedMsec) + " " + logLine;

    qDebug() << line;
}


void Logger::ClearLogHysteresisHistory()
{
    m_errorMap.clear();
}

void Logger::AddLogfilter(QString filter)
{
    if (!filter.isEmpty())
        m_logFilter.append(filter);

    QString log("Filter debug logs by:\n");
    foreach (QString f, m_logFilter) {
        log.append(f + "\n");
    }
    AddLog(log);
}

void Logger::ClearLogFilter()
{
    m_logFilter.clear();
    AddLog("Clear log filter");
}


Logger *Logger::Instance()
{
    return &LoggerInstance;
}

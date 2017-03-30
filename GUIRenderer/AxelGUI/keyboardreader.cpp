#include "keyboardreader.h"

#include <QTextStream>
#include <logger.h>
#include <QTimer>
#include <QApplication>

KeyboardReader::KeyboardReader(ILayoutManager * manager)
{
    _manager = manager;
}

KeyboardReader::~KeyboardReader()
{
    _manager = NULL;
}

//void KeyboardReader::StartReading()
//{
//    QTimer::singleShot(1, this, SLOT(processInput()));

//}


void KeyboardReader::processInput()
{
    while (true) {

        QTextStream s(stdin);
        QString value = s.readLine();

        Logger::Instance()->AddLog("Read Command: " + value, LoggerLevel_DEBUG);
        QStringList cmd = value.split(" ", QString::SkipEmptyParts);
        if (cmd.length() < 1)
        {
            PrintUsage();
            continue;
        }
        if((cmd.at(0) == "v") && (cmd.count() >= 2))
        {
            bool bOk;
            int setLevel = cmd.at(1).toInt(&bOk);
            if(!bOk)
            {
                Logger::Instance()->AddLog("Error parsing log level", LoggerLevel_ERROR);
                PrintUsage();
                continue;
            }
            if((setLevel < 0) || (setLevel > 4))
            {
                Logger::Instance()->AddLog("Log level " + QString::number(setLevel) + " not valid", LoggerLevel_ERROR);
                PrintUsage();
                continue;
            }

            Logger::Instance()->setMaxLogLevel((LoggerLevel) setLevel);
            switch(setLevel)
            {
            case 0:
                Logger::Instance()->AddLog("Set log level: INFO");
                break;
            case 1:
                Logger::Instance()->AddLog("Set log level: WARNING");
                break;
            case 2:
                Logger::Instance()->AddLog("Set log level: ERROR");
                break;
            case 3:
                Logger::Instance()->AddLog("Set log level: DEBUG");
                break;
            case 4:
                Logger::Instance()->AddLog("Set log level: DEBUG VERBOSE");
                break;
            }
        }
        else if(cmd.at(0) == "h")
        {
            if ((cmd.count() == 1) && Logger::Instance()->logHysteresisEnabled())
            {
                Logger::Instance()->AddLog("Disable log hysteresis");
                Logger::Instance()->setLogHysteresisEnabled(false);
            }
            else
            {
                if(cmd.count() > 1)
                {
                    bool bOk;
                    long hysteresisValue = cmd.at(1).toInt(&bOk);
                    if(!bOk)
                    {
                        Logger::Instance()->AddLog("Error parsing hysteresis level", LoggerLevel_ERROR, "HYSTERESYS");
                        continue;
                    }
                    Logger::Instance()->AddLog("Set hysteresis value=" + QString::number(hysteresisValue));
                    Logger::Instance()->setLogHysteresisValue(hysteresisValue);
                }

                if(!Logger::Instance()->logHysteresisEnabled())
                {
                    Logger::Instance()->AddLog("Enable log hysteresis");
                    Logger::Instance()->setLogHysteresisEnabled(true);
                }
            }
        }
        else if(cmd.at(0) == "c")
        {
            Logger::Instance()->AddLog("Clear log hysteresis history");
            Logger::Instance()->ClearLogHysteresisHistory();
        }
        else if(cmd.at(0) == "dp")
        {
            PrintDebugInfo();
        }
        else if(cmd.at(0) == "dl")
        {
            PrintDebugLayout();
        }
        else if(cmd.at(0) == "dr")
        {
            if (_manager == 0)
                continue;
            Logger::Instance()->AddLog("Clear debug info");
            _manager->ResetDebugInfo();
        }
        else if(cmd.at(0) == "f")
        {
            QString filter;
            if(cmd.count() >= 2)
                filter = cmd.at(1);
            Logger::Instance()->AddLogfilter(filter);
        }
        else if(cmd.at(0) == "fc")
        {
            Logger::Instance()->ClearLogFilter();
        }
        else if(cmd.at(0) == "q")
        {
            Logger::Instance()->AddLog("Exit application");
            emit finished();
            QApplication::quit();
        }
        else
        {
            PrintUsage();
        }
    }
}


void KeyboardReader::PrintUsage()
{
    qDebug() << "Available commads: \n"
             << " v [0|1|2|3|4] set verbosity INFO=0, WARNING=1, ERROR=2, DEBUG=3, DEBUGVERBOSE=4\n"
             << " h             toggle log hysteresis\n"
             << " h N           set hysteresis msec value\n"
             << " c             clear hysteresis history\n"
             << " dp            debug: print info\n"
             << " dl            debug: print last loaded layout json\n"
             << " dr            debug: reset info\n"
             << " f [filter]    print only debug log lines containing \"filter\" (use multiple times to add filters)\n"
             << " fc            log filter clear\n"
             << " q             quit\n";
}


void KeyboardReader::PrintDebugInfo()
{
    if(_manager == 0)
        return;

    qDebug() << "Debug info: \n"
             << " version: " << _manager->GetVersion() << "\n"
             << " last loaded layout = " << _manager->GetLastLayoutId() << "\n"
             << " loaded layouts counter = " << QString::number(_manager->GetLayoutCounter()) << "\n";
}


void KeyboardReader::PrintDebugLayout()
{
    if(_manager == 0)
        return;

    qDebug() << "Debug last loaded layout: \n"
             << _manager->GetLastLayoutJsonData();
}

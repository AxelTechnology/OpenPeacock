/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>
#include <QtWidgets>
#include <logger.h>

#include "axelframe.h"
#include "guilayoutmanager.h"
#include "keyboardreader.h"


int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    QDesktopWidget widget;
    int screenHeight = widget.height();
    int screenWidth = widget.width();

#ifdef ON_DESKTOP
    screenWidth = 800;
    screenHeight = 480;    
#endif


    QFile *inFile = 0;
    int port = 8090;

    bool forceDesktop = false;
    bool demoLayout = false;

    //Command line parsing
    QString arg;
    for (int i = 0; i < QCoreApplication::arguments().count(); i++)
    {
        arg = QCoreApplication::arguments().at(i);

        if (arg == "-h")
        {
            GuiLayoutManager::PrintUsage();
            return 0;
        }

        if (arg == "-v")
            Logger::Instance()->setMaxLogLevel(LoggerLevel_DEBUG);
        else if (arg == "-vv")
            Logger::Instance()->setMaxLogLevel(LoggerLevel_DEBUGVERBOSE);
        else if(QFileInfo(arg).suffix() == "json")        
            inFile = new QFile(arg);
        else if(arg == "-p")
        {
            if(i >= QCoreApplication::arguments().count() - 1)
            {
                GuiLayoutManager::PrintUsage();
                return 0;
            }
            QString sPort = QCoreApplication::arguments().at(i + 1);
            bool ok = false;
            port = sPort.toInt(&ok);
            if (!ok)
            {
                GuiLayoutManager::PrintUsage();
                return 0;
            }
        }
        else if(arg == "-f")
            forceDesktop = true;
        else if(arg == "-d")
            demoLayout = true;
    }


    AxelFrame * bgFrame = new AxelFrame(0, forceDesktop);
    bgFrame->setGeometry(0,0,screenWidth,screenHeight);
    bgFrame->setStyleSheet("background-color:rgb(0, 0, 0);");



    GuiLayoutManager *layout = new GuiLayoutManager((QWidget*)bgFrame);
    if(!layout->Initialize(port, demoLayout))
    {        
        Logger::Instance()->AddLog("Error initializing layout", LoggerLevel_ERROR, "LAYOUT_INIT");
    }
    Logger::Instance()->AddLog("Listening on port " + QString::number(port), LoggerLevel_INFO);

    if(inFile != 0)
    {
        if (inFile->exists()) // && false)
        {
            if (inFile->open(QFile::ReadOnly | QFile::Text))
            {
                QTextStream in(inFile);
                layout->setData(in.readAll());
                inFile->close();
            }
        }
    }


    //Layout debug
    /*
    QString jsonData = layout->GetLayout();

    QFile outFile("document_out.json");
    if(outFile.exists())
        outFile.remove();
    if(outFile.open(QFile::ReadWrite | QFile::Text))
    {
        QTextStream out(&outFile);
        out << jsonData;
        outFile.close();
    }
    */

    layout->ShowLayout();



    int retVal = 0;
    retVal = app.exec();

    return retVal;
}




#include "axeluiloader.h"
#include "mainwindow.h"
#include <QtUiTools/QtUiTools>
#include <QtUiTools/quiloader.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QUdpSocket>
#include <logger.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    currentLayout = 0;
    initialized = false;
    Initialize();

    serializer = new JsonSerializer(ui->frameLayout);

    //loadUiFile(":/testwindow.ui");
}


void MainWindow::Initialize()
{
    foreach( const QFileInfo &fi, QDir(":").entryInfoList())
    {
        Logger::Instance()->AddLog("Enumerating resource: " + fi.completeBaseName(), LoggerLevel_DEBUG);

        if( fi.suffix() != "ui")
            continue;

        QString name = fi.completeBaseName();
        QString path = fi.filePath();
        ui->cmbLayouts->addItem(name, path);

        Logger::Instance()->AddLog("Added resource: " + fi.completeBaseName(), LoggerLevel_DEBUG);
    }

    ui->cmbLayouts->setCurrentIndex(5);

    initialized = true;

    LoadComboLayout();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ShowCurrentLayout(QString uiFile)
{
    if(currentLayout != 0)
    {
        delete currentLayout;
        currentLayout = 0;
    }

    currentLayout = loadUiFile(uiFile);
    if(currentLayout == 0)
        return;
    currentLayout->setParent(ui->frameLayout);
    currentLayout->show();

}

QWidget * MainWindow::loadUiFile(QString uiFile)
{
    if (!QFile::exists(uiFile))
        return 0;

    QWidget* retLayout;

    AxelUiLoader loader(ui->frameLayout);
    //QUiLoader loader(ui->frameLayout);
    QFile file(uiFile);
    file.open(QFile::ReadOnly);

    retLayout = loader.load(&file, this);
    file.close();

    return retLayout;
}



void MainWindow::on_cmbLayouts_currentIndexChanged(int)
{
    LoadComboLayout();
}

void MainWindow::LoadComboLayout()
{
    if(!initialized)
        return;

    QString layoutPath = ui->cmbLayouts->itemData(ui->cmbLayouts->currentIndex()).toString();
    if (!QFileInfo::exists(layoutPath))
        return;

    ShowCurrentLayout(layoutPath);
}



void MainWindow::on_btnExport_released()
{
    if(currentLayout == 0)
        return;
    if(serializer == 0)
        return;

    const QString DEFAULT_DIR_KEY("default_dir");
    QSettings MySettings;
    QString defaultDir = MySettings.value(DEFAULT_DIR_KEY).toString();

    QList<QWidget*> * widgetList = serializer->getSerializableWidgets(currentLayout);
    QString layoutName = ui->cmbLayouts->itemText(ui->cmbLayouts->currentIndex()).toLower();

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setAutoFillBackground(false);
    dialog.setDefaultSuffix("json");
    QString suggestFile(QDir(defaultDir).absoluteFilePath(layoutName + ".json"));

    //QString filePath = dialog.getSaveFileName(this, "Select file", QDir::homePath(), "json Files (*.json)");
    QString filePath = dialog.getSaveFileName(this, "Select file", suggestFile, "json Files (*.json)");

    if(filePath.isEmpty())
        return;



    QFileInfo fi(filePath);
    if (fi.suffix() != "json")
    {
        filePath = filePath.replace("." + fi.suffix(), "");
        filePath.append(".json");
    }
    WriteWidgetsToFile(filePath, currentLayout, widgetList, true);

    MySettings.setValue(DEFAULT_DIR_KEY, fi.absolutePath());

}


void MainWindow::on_btnExportAll_released()
{
    if(!initialized)
        return;

    const QString DEFAULT_DIR_KEY("default_dir");
    QSettings MySettings;

    //QString dir = QFileDialog::getExistingDirectory(this, "Select directory", QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QString dir = QFileDialog::getExistingDirectory(this, "Select directory", MySettings.value(DEFAULT_DIR_KEY).toString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!QFileInfo::exists(dir))
        return;

    for(int i = 0; i < ui->cmbLayouts->count(); i++)
    {
        QString layoutPath = ui->cmbLayouts->itemData(i).toString();
        QString layoutName = ui->cmbLayouts->itemText(i).toLower();

        if(layoutName.startsWith("test"))
            continue;

        if (!QFileInfo::exists(layoutPath))
            return;

        //QWidget* layoutData = loadUiFile(layoutPath);
        ShowCurrentLayout(layoutPath);
        QWidget* layoutData = currentLayout;

        QList<QWidget*> * widgetList = serializer->getSerializableWidgets(layoutData);

        QString filePath = dir + QDir::separator() + layoutName + ".json";

        WriteWidgetsToFile(filePath, layoutData, widgetList, true);

    }

    QMessageBox msgBox;
    msgBox.setText("Done exporting all layouts.");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
}




void MainWindow::WriteWidgetsToFile(QString outFilePath, QWidget * parent, QList<QWidget*> * widgets, bool varsInSubdir)
{
    GUIData *data = new GUIData();
    data->setTotalSize(QSize(parent->width(), parent->height()));
    data->setWidgetList(widgets);

    QString jsonData = serializer->SerializeData(data);

    //Write widgets layout data
    QFile outFile(outFilePath);
    if(outFile.exists())
        outFile.remove();
    if(outFile.open(QFile::ReadWrite | QFile::Text))
    {
        QTextStream out(&outFile);
        out << jsonData;
        outFile.close();
    }

    //Write wdigets variables data
    QFileInfo fi(outFilePath);    
    QString variablesFilePath = outFilePath.replace("." + fi.suffix(), "_vars." + fi.suffix());
    if(varsInSubdir)
    {
        QString subDirPath = fi.absolutePath(); // + QDir::separator() + "vars";
        if(!QDir(subDirPath).exists())
            QDir().mkdir(subDirPath);
        variablesFilePath = variablesFilePath.replace(fi.absolutePath(), subDirPath);
    }

    jsonData = serializer->SerializeVariablesData(data);
    QFile outFileVars(variablesFilePath);
    if(outFileVars.exists())
        outFileVars.remove();
    if(outFileVars.open(QFile::ReadWrite | QFile::Text))
    {
        QTextStream out(&outFileVars);
        out << jsonData;
        outFileVars.close();
    }

}





void MainWindow::on_btnTest_released()
{
    if(currentLayout == 0)
        return;
    if(serializer == 0)
        return;

    QList<QWidget*> * widgetList = serializer->getSerializableWidgets(currentLayout);

    GUIData *data = new GUIData();
    data->setTotalSize(QSize(currentLayout->width(), currentLayout->height()));
    data->setWidgetList(widgetList);

    QString jsonData = serializer->SerializeData(data);

    QUdpSocket *_socket;
    _socket = new QUdpSocket(this);

    //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 2124);
    _socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 2140);
    //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 8090);
    _socket->writeDatagram(jsonData.toUtf8(), QHostAddress("127.0.0.1"), 8090);

    //_socket->writeDatagram("{}", QHostAddress("192.168.99.150"), 2124);
    _socket->writeDatagram("{}", QHostAddress("192.168.99.150"), 2140);
    //_socket->writeDatagram("{}", QHostAddress("192.168.99.150"), 8090);
    _socket->writeDatagram("{}", QHostAddress("127.0.0.1"), 8090);
}



void MainWindow::on_btnTestMessage_released()
{
    QUdpSocket *_socket;
    _socket = new QUdpSocket(this);


    //GRAPH TEST
    QString jsonData(" { "
       "\"id\": \"qGraph\","
       "\"property\": \"graphs\","
       "\"value\": ["
          "{"
             "\"filtertype\": 4,"
             "\"samplerate\": 48000,"
             "\"gain\": 0.1,"
             "\"frequency\": 100,"
             "\"selected\": true,"
             "\"A0\": 1.0000752924129819,"
             "\"A1\": -1.9868252854194832,"
             "\"A2\": 0.9869202237328067,"
             "\"B1\": -1.9868252854194832,"
             "\"B2\":  0.9869955161457885"
          "},"
         "{"
            "\"filtertype\": 0,"
            "\"samplerate\": 48000,"
            "\"gain\": 14,"
            "\"frequency\": 200,"
            "\"selected\": true,"
            "\"A0\": 1.026083944176506,"
            "\"A1\": -1.986315728911407,"
            "\"A2\": 0.960912678851265,"
            "\"B1\": -1.986315728911407,"
            "\"B2\": 0.9869966230277707"
         "},"
          "{"
             "\"filtertype\": 4,"
             "\"samplerate\": 48000,"
             "\"gain\": 7,"
             "\"frequency\": 850,"
             "\"selected\": false,"
             "\"A0\": 1.0393084869699332,"
             "\"A1\": -1.9245591293208173,"
             "\"A2\": 0.897225272330624,"
             "\"B1\": -1.9245591293208173,"
             "\"B2\": 0.9365337593005572"
          "},"
         "{"
            "\"filtertype\": 4,"
            "\"samplerate\": 48000,"
            "\"gain\": 16,"
            "\"frequency\": 3000,"
            "\"selected\": false,"
             "\"A0\": 1.5388640618331821,"
             "\"A1\": -1.6602315799207472,"
             "\"A2\": 0.2581576509521529,"
             "\"B1\": -1.6602315799207472,"
             "\"B2\": 0.7970217127853352"
         "},"
         "{"
            "\"filtertype\": 4,"
            "\"samplerate\": 48000,"
            "\"gain\": -10,"
            "\"frequency\": 8000,"
            "\"selected\": false,"
            "\"A0\": 0.5943806704341423,"
            "\"A1\": -0.40679175110253757,"
            "\"A2\": 0.21920283177093272,"
            "\"B1\": -0.40679175110253757,"
            "\"B2\": -0.18641649779492478"

         "}"

       "]"
    "} ");

    //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 2124);
//    _socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 8090);
    //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("127.0.0.1"), 8090);

    //jsonData = QString("{\"id\":\"qGraph\",\"key\":null,\"properties\":null,\"property\":\"graphs\",\"value\":[{},{},{\"A0\":1.3927750849629037,\"A1\":-0.40380742610467896,\"A2\":0.16741700192439218,\"B1\":-0.40380742610467896,\"B2\":0.56019208688729583,\"filtertype\":4,\"frequency\":10000,\"gain\":8.8999996185302734,\"samplerate\":48000,\"selected\":true}]}");
    //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("127.0.0.1"), 8090);

    jsonData = QString("{\"id\":\"qGraph\",\"key\":null,\"properties\":null,\"property\":\"graphs\",\"value\":[{},{\"A0\":0.95402367355861539,\"A1\":-1.8406824123454499,\"A2\":0.90254191476354395,\"B1\":-1.8406824123454499,\"B2\":0.85656558832215923,\"filtertype\":4,\"frequency\":1000,\"gain\":-8.8999996185302734,\"samplerate\":48000,\"selected\":true}]}");
    _socket->writeDatagram(jsonData.toUtf8(), QHostAddress("127.0.0.1"), 8090);
    _socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 2124);

    //VUMETER TEST SINGLE COMMAND
    //vuMeterTestSingle(_socket);

    //VUMETER TEST COMMAND ARRAY
    //vuMeterTestArray(_socket);

    //PANEL BLINKING TEST
    //panelTest(_socket);

    //WHEEL TEST
//    for(int i = 0; i< 50; i++)
//    {
//        for (int j = 1; j<=3; j++)
//        {
//            if((j == 1) && (i > 30))
//                continue;
//            if((j == 2) && (i > 40))
//                continue;

//            QString jsonData(" { "
//               "\"id\": \"qWheel_" + QString::number(j) + "\","
//               "\"property\": \"value\","
//               "\"value\": " + QString::number(qrand() % 10) +
//            "} ");

//            //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 2124);
//            _socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 8090);
//            //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("127.0.0.1"), 8090);
//        }

//        QThread::msleep(100);
//    }

//        QString jsonDataWheel1(" { "
//           "\"id\": \"qWheel\","
//           "\"property\": \"value\","
//           "\"value\": 3"
//        "} ");
//        QString jsonDataWheel2(" { "
//           "\"id\": \"qWheel\","
//           "\"property\": \"value\","
//           "\"value\": 2"
//        "} ");

//        //_socket->writeDatagram(jsonDataKnob1.toUtf8(), QHostAddress("192.168.99.150"), 2124);
//        //_socket->writeDatagram(jsonDataKnob1.toUtf8(), QHostAddress("192.168.99.150"), 8090);
//        _socket->writeDatagram(jsonDataWheel1.toUtf8(), QHostAddress("127.0.0.1"), 8090);

//        QThread::msleep(1000);

//        _socket->writeDatagram(jsonDataWheel2.toUtf8(), QHostAddress("127.0.0.1"), 8090);

    QString wheelTest(  "{"
                        "\"id\":\"wheelSource\","
                        "\"properties\":["
                                "{"
                                    "\"property\": \"value\","
                                    "\"type\": \"INTEGER\","
                                    "\"value\": 2"
                                "},"
                                "{"
                                    "\"property\": \"values\","
                                    "\"type\": \"STRINGARRAY\","
                                    "\"value\": ["
                                        "\"PGM\","
                                        "\"SUB\","
                                        "\"AUX-1\","
                                        "\"AUX-2\","
                                        "\"Tone-Gen\""
                                    "]"
                                "}"
                            "]"
                        "}");
    //_socket->writeDatagram(wheelTest.toUtf8(), QHostAddress("127.0.0.1"), 8090);

    //KNOB TEST
//    QString jsonData(" { "
//       "\"id\": \"qknob\","
//       "\"property\": \"value\","
//       "\"value\": " + QString::number(0.01) +
//    "} ");
//    //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 2124);
//    //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 8090);
//    _socket->writeDatagram(jsonData.toUtf8(), QHostAddress("127.0.0.1"), 8090);


    //MESSAGEBOX TEST
    QString msgData;
    if(!msgTestVisible)
    {
        msgData = QString(" { "
           "\"key\": \"SHOW_MSGBOX\","
           "\"text\": \"Sei tu John Wayne?\","
            "\"type\": 3,"
            "\"timeout\": 10000"
        "} ");
    }
    else
    {
        msgData = QString(" { "
           "\"key\": \"SHOW_MSGBOX\","
           "\"text\": \"This is not a test of the emergency broadcast system\","
            "\"type\": 0,"
            "\"timeout\": 0"
        "} ");
    }
    msgTestVisible = !msgTestVisible;
    //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 2124);
    //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 8090);
    //_socket->writeDatagram(msgData.toUtf8(), QHostAddress("127.0.0.1"), 8090);


    _socket->close();
    delete _socket;
}

void MainWindow::vuMeterTestSingle(QUdpSocket * _socket)
{
    for(int i = 0; i< 100; i++)
    {
        int maxVumeterIndex = 15;
        QString jsonData;

        for (int j = 1; j<= maxVumeterIndex; j++)
        {
            jsonData = QString(" { "
               "\"id\": \"vuMeter_" + QString::number(j) + "\","
               "\"property\": \"valueAPerc\","
               "\"value\": " + QString::number(qrand() % 100) +
            "} ");

            //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 2124);
            //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 8090);
            //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("127.0.0.1"), 8090);


            jsonData = QString (" { "
               "\"id\": \"vuMeter_" + QString::number(j) + "\","
               "\"property\": \"valueBPerc\","
               "\"value\": " + QString::number(qrand() % 100) +
            "} ");

            //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 2124);
            //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 8090);
            //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("127.0.0.1"), 8090);
        }


        Logger::Instance()->AddLog("sending " + jsonData, LoggerLevel_DEBUGVERBOSE);

        QThread::msleep(30);
    }
}

void MainWindow::vuMeterTestArray(QUdpSocket * _socket)
{
    for(int i = 0; i< 100; i++)
    {
        int minVumeterIndex = 1;
        int maxVumeterIndex = 15;
        QString jsonData("[");

        for (int j = minVumeterIndex; j<= maxVumeterIndex; j++)
        {
            jsonData += QString(" { "
               "\"id\": \"vuMeter_" + QString::number(j) + "\","
               "\"property\": \"valueAPerc\","
               "\"value\": " + QString::number(qrand() % 100) +
            "}, ");

            jsonData += QString (" { "
               "\"id\": \"vuMeter_" + QString::number(j) + "\","
               "\"property\": \"valueBPerc\","
               "\"value\": " + QString::number(qrand() % 100) +
            "} ");

            if (j != maxVumeterIndex)
                jsonData += ", ";
            else
                jsonData += " ]";
        }

        //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 2124);
        //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 8090);
        _socket->writeDatagram(jsonData.toUtf8(), QHostAddress("127.0.0.1"), 8090);

        Logger::Instance()->AddLog("sending " + jsonData, LoggerLevel_DEBUGVERBOSE);

        QThread::msleep(50);
    }
}

void MainWindow::panelTest(QUdpSocket * _socket)
{
    int maxPanelIndex = 16;
    QString jsonData("[");

    for (int j = 1; j<= maxPanelIndex; j++)
    {
        jsonData += QString(" { "
           "\"id\": \"axelpanel" + QString::number(j) + "\","
            "\"property\": \"BLINK\","
            "\"value\": 30"
        "}");

        if (j != maxPanelIndex)
            jsonData += ", ";
        else
            jsonData += " ]";
    }

    //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 2124);
    _socket->writeDatagram(jsonData.toUtf8(), QHostAddress("192.168.99.150"), 8090);
    //_socket->writeDatagram(jsonData.toUtf8(), QHostAddress("127.0.0.1"), 8090);

    Logger::Instance()->AddLog("sending " + jsonData, LoggerLevel_DEBUGVERBOSE);

    //QThread::msleep(120);

}



#include "widgetfactory.h"
#include "multiimage.h"
#include "onairclockwidget.h"
#include "graphconfiguration.h"
#include "graphdata.h"
#include "qgraph.h"
#include "guilayoutmanager.h"
#include "axellabel.h"
#include "qknob.h"
#include "vumeter.h"
#include "knobconfiguration.h"
#include "qwheel.h"
#include "wheelconfiguration.h"
#include "axelframe.h"
#include "guidata.h"
#include "axelpanel.h"
#include "logger.h"
#include "keyboardreader.h"
//#include <QQuickWidget>
//#include <qmlwidget.h>


const QString GuiLayoutManager::_version = QString("1.5.5");

GuiLayoutManager::GuiLayoutManager(QWidget *parent)
{
    _parent = parent;
    _serializer = new JsonSerializer(parent);

    _layoutLoadCounter = 0;

    _messagebox = 0;
}

bool GuiLayoutManager::Initialize(int listenPort, bool setDemoLayout)
{    
    Logger::Instance()->AddLog("AxelGUI v" + _version + " started...", LoggerLevel_INFO);

    QThread * keyboardThread = new QThread();
    keyboardThread->setObjectName("AxelGUI_KeyboardReader");
    KeyboardReader* kReader = new KeyboardReader(this);
    kReader->moveToThread(keyboardThread);
    //kReader.StartReading();
    connect(keyboardThread, SIGNAL(started()), kReader, SLOT(processInput()));
    connect(kReader, SIGNAL(finished()), keyboardThread, SLOT(quit()));
    connect(keyboardThread, SIGNAL(finished()), keyboardThread, SLOT(deleteLater()));
    keyboardThread->start();

    _timeSynchronizerSettingsFilePath = QDir::cleanPath(QCoreApplication::applicationDirPath() + QDir::separator() + "time-sync-settings.json");
    Logger::Instance()->AddLog("Time synchrnoizer settings: " + _timeSynchronizerSettingsFilePath);

    _timeSynchronizer = new TimeSynchronizer();
    _timeSynchronizerThread = new QThread();
    _timeSynchronizerThread->setObjectName("AxelGUI_TimeSynchronizer");
    _timeSynchronizerThread->start();
    _timeSynchronizer->moveToThread(_timeSynchronizerThread);
    _timeSynchronizer->StartSync(_timeSynchronizerSettingsFilePath);

    _networkManager = new NetworkManager();
    connect(_networkManager, SIGNAL(ConfigStatusUpdate(SystemManagerEnums::NetworkConfigurationStatus)), this, SLOT(NetworkManagerStatusUpdate(SystemManagerEnums::NetworkConfigurationStatus)));
    _networkManagerThread = new QThread();
    _networkManagerThread->setObjectName("AxelGUI_NetworkManager");
    _networkManagerThread->start();
    _networkManager->moveToThread(_networkManagerThread);

    _widgetList = new QList<QWidget *>();
    _socket = new UdpSocket(this);
    connect(_socket, SIGNAL(dataArrived(QString)), this, SLOT(setData(QString)));

    if(setDemoLayout)
        SetupTestLayout();
    else
        SetupSplashLayout();

    WidgetFactory::Initialize();

    _socket->Start(listenPort);


    return true;
}

QList<QWidget*>* GuiLayoutManager::widgetList() const
{
    return _widgetList;
}

bool GuiLayoutManager::ShowLayout()
{
    Logger::Instance()->AddLog("Start show layout", LoggerLevel_DEBUG);


    Logger::Instance()->AddLog("Showing non AbstractWidgets", LoggerLevel_DEBUG);

    //Showing firstly non AbstractWidgets
    for (int i = 0; i < _widgetList->length(); i ++)
    {
        if(!_widgetList->at(i)->inherits("AbstractWidget"))
            _widgetList->at(i)->show();
    }


    Logger::Instance()->AddLog("Showing AbstractWidgets ignoring order", LoggerLevel_DEBUG);
    for (int i = 0; i < _widgetList->length(); i ++)
    {
        if(!_widgetList->at(i)->inherits("AbstractWidget"))
            continue;
        AbstractWidget* w = (AbstractWidget*)_widgetList->at(i);
        if(w->objectName() == "lblGuiVersion")
        {
            AxelLabel* lbl = (AxelLabel*) w;
            lbl->setTextPrimary(_version);
        }

        if(w->widgetVisible())
            w->show();
        else
            w->hide();

        //QString name = QString(_widgetList->at(i)->metaObject()->className());
        //Logger::Instance()->AddLog("Showed " + name, LoggerLevel_DEBUGVERBOSE);
    }

    _parent->update();

    if(_messagebox != 0)
        if(_messagebox->isVisible())
            _messagebox->raise();

    Logger::Instance()->AddLog("End show layout", LoggerLevel_DEBUG);

    return true;
}

bool GuiLayoutManager::ClearLayout()
{
    QList<QWidget *> * deleteList = new QList<QWidget *>();
    for (int i = 0; i < _widgetList->length(); i ++)
    {
        if(_widgetList->at(i) == 0)
            continue;
        if(_widgetList->at(i)->inherits("AbstractWidget"))
        {
            AbstractWidget * aw = (AbstractWidget*) _widgetList->at(i);
            if (!aw->parentObjectName().isEmpty())
                continue;
        }

        deleteList->append(_widgetList->at(i));

//        _widgetList->at(i)->hide();
//        delete _widgetList->at(i);

    }
    _widgetList->clear();

     for (int i = 0; i < deleteList->length(); i ++)
     {
         Logger::Instance()->AddLog("Deinitializing widget " + deleteList->at(i)->objectName(), LoggerLevel_DEBUG);
         deleteList->at(i)->hide();
         delete deleteList->at(i);
     }

     deleteList->clear();

     return true;
}

bool lock;
//Parsing json data. "key" value can be:
// - "SCREEN_ROTATION" -> set a layout rotation in degrees
// - "SCREEN_LAYOUT" -> a full layout description (all current data will be cleared)
// - other -> id of the widget which will receive the command set in "value"
void GuiLayoutManager::setData(QString data)
{
    if (lock)    
        return;    
    lock = true;

    setDataPrivate(data);

    lock = false;

}

//Json input data can be a single object or an array of objects
void GuiLayoutManager::setDataPrivate(QString data)
{
    if (data.isEmpty() || data.isNull())
        return;
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    if (!doc.isObject() && !doc.isArray())
    {
        Logger::Instance()->AddLog("Document is not an object or array: " + data.toUtf8(), LoggerLevel_ERROR, "JSON_OBJ");
        return;
    }

    if(doc.isObject())
        setDataObject(doc.object());
    else if(doc.isArray())
    {
        QJsonArray commands = doc.array();

        foreach (QJsonValue c, commands) {
            if (!c.isObject())
                continue;
            setDataObject(c.toObject());
        }
    }

}


void GuiLayoutManager::setDataObject(QJsonObject obj)
{
    QString key = obj["key"].toString();
    QString id = obj["id"].toString();

    if(QString::compare(key, "SCREEN_ROTATION") == 0)
    {
        Logger::Instance()->AddLog("received command -> SCREEN_ROTATION", LoggerLevel_DEBUG);
        QString val = obj["value"].toString();
        int rotation = val.toInt();
        ((AxelFrame*) _parent)->Rotate(rotation);
        return;
    }
    else if(QString::compare(key, "SCREEN_LAYOUT") == 0)
    {
        GUIData* guiData = _serializer->DeserializeData(obj);
        if(guiData == 0)
        {
            Logger::Instance()->AddLog("error parsing layout", LoggerLevel_ERROR, "JSON_PARSE");
            return;
        }

        Logger::Instance()->AddLog("Start setting layout", LoggerLevel_INFO);
        SetLayout(guiData);
        _layoutShowing = false;
        Logger::Instance()->AddLog("End setting layout", LoggerLevel_INFO);
        lock = false;

        _lastLoadedLayout = id;
        QJsonDocument doc(obj);
        _lastLoadedLayoutJsonData = QString(doc.toJson());

        if(_layoutLoadCounter >= std::numeric_limits<qulonglong>::max() )
            _layoutLoadCounter = 0;
        _layoutLoadCounter++;

        return;
    }
    else if(QString::compare(key, "SHOW_MSGBOX") == 0)
    {
        QString text = obj["text"].toString();
        int type = obj["type"].toInt();
        int timeout = obj["timeout"].toInt();

        if(timeout == 0)
            HideMessagebox();
        else
            ShowMessagebox(text, (AxelMessagebox::MessageboxType) type, timeout);

        return;
    }
    else if(QString::compare(key, "SET_DATE") == 0)
    {
        TimeSynchronizerSettings* timeSyncSettings = _timeSynchronizer->settings();
        if(timeSyncSettings == 0)
            return;
        timeSyncSettings->SetJsonObj(obj);
        return;
    }
    else if(QString::compare(key, "NETWORK_SETTINGS") == 0)
    {
        if (_networkManager == NULL)
            return;
        _networkManager->SetJsonObj(obj);
        return;
    }
    else
    {
        QJsonValue properties = obj["properties"];
        if (properties.isArray())
        {
            QJsonArray propertiesArray = properties.toArray();

            foreach (QJsonValue p, propertiesArray) {
                if (!p.isObject())
                    continue;
                SetWidgetDataJson(id, p.toObject());
            }
        }
        else
        {
            SetWidgetDataJson(id, obj);
        }

        if(!_layoutShowing)
        {
            Logger::Instance()->AddLog("FIRST UDPATE RECEIVED", LoggerLevel_DEBUG);
            ShowLayout();
            _layoutShowing = true;
        }
    }
}


void GuiLayoutManager::ShowMessagebox(QString message, AxelMessagebox::MessageboxType type, int timeoutMsec)
{
    Logger::Instance()->AddLog("Showing messagebox: " + message, LoggerLevel_INFO);

    if(_messagebox != 0)
    {
        _messagebox->hide();
        delete _messagebox;
    }
    _messagebox = new AxelMessagebox(_parent);
    _messagebox->ShowMessage(message, type, timeoutMsec);
}


void GuiLayoutManager::HideMessagebox()
{
    if(_messagebox == 0)
        return;
    Logger::Instance()->AddLog("Hiding messagebox", LoggerLevel_INFO);
    _messagebox->hide();
}


void GuiLayoutManager::SetWidgetDataJson(QString id, QJsonObject obj)
{
    if(id == "qGraph")
    {
        Logger::Instance()->AddLog("");
    }
    QString property = obj["property"].toString();
    QJsonValue value = obj["value"];    

    bool init = false;
    int iInit = obj["init"].toInt();
    if(iInit > 0)
    {
        init = true;
        Logger::Instance()->AddLog("");
    }
    QJsonDocument doc(obj);
    Logger::Instance()->AddLog("received command -> id=" + id + " " + doc.toJson(QJsonDocument::Compact), LoggerLevel_DEBUG, "SET_JSONDATA_" + id + "_" + property);
    SetWidgetData(id, property, value, init);
}



void GuiLayoutManager::SetLayout(GUIData* data)
{
    if(data == 0)
        return;


    ClearLayout();
    Logger::Instance()->AddLog("Cleared layout", LoggerLevel_DEBUG);

    _parent->setGeometry(0,0,data->totalSize().width(), data->totalSize().height());

    Logger::Instance()->AddLog("Set main geometry", LoggerLevel_DEBUG);

    _widgetList = data->widgetList();

}

QString GuiLayoutManager::GetLayout()
{
    GUIData* data= new GUIData();
    data->setTotalSize(QSize(_parent->geometry().width(),_parent->geometry().height()));
    data->setWidgetList(_widgetList);
    return _serializer->SerializeData(data);
}




void GuiLayoutManager::SetWidgetData(QString id, QString property, QJsonValue value, bool init)
{
    //Logger::Instance()->AddLog("SetWidgetData id=" + id + " property=" + property + " value=" + value.toString(), LoggerLevel_DEBUG);

    for (int i = 0; i < _widgetList->length(); i ++)
    {
        if (!_widgetList->at(i)->inherits("AbstractWidget"))
        {
            Logger::Instance()->AddLog("Widget " + QString::number(i) + " is not an AbstractWidget", LoggerLevel_ERROR, "WIDGET_TYPE");
            continue;
        }
        AbstractWidget *widget = (AbstractWidget*) _widgetList->at(i);

        QString myId;
        try
        {
            //myId = widget->id();
            myId = widget->objectName();
        }
        catch(...)
        {
            Logger::Instance()->AddLog("Error getting mixer id", LoggerLevel_ERROR, "WIDGET_ID");
            continue;
        }

        if (myId == 0)
            continue;
        if (QString::compare(myId, id, Qt::CaseSensitive) == 0)
        {
            QString error = NULL;
            if(!widget->ManageRemoteCommand(property, value, init, &error))
                Logger::Instance()->AddLog("Error setting widget data on " + id + " property=" + property + error, LoggerLevel_ERROR, "WIDGET_DATA");

        }                
    }
}

void GuiLayoutManager::SetupSplashLayout()
{
    Logger::Instance()->AddLog("Creating test layout", LoggerLevel_DEBUG);

//    if(QFile::exists("Axel_flash.gif"))
//    {
//        MultiImage* imgLoading = new MultiImage(_parent);
//        imgLoading->setGeometry(0,0,800,480);
//        QStringList ldList = QStringList();
//        ldList << "Axel_flash.gif";
//        imgLoading->setImageList(ldList);
//        imgLoading->setShowImageIndex(0);
//        _widgetList->append(imgLoading);
//    }
//    else
//    {

        MultiImage *img = new MultiImage(_parent);
        img->setGeometry(150, 40, 480, 140 );
        QStringList imagesList = QStringList();
        imagesList << ":/axel_logo.png";
        img->setImageList(imagesList);
        img->setShowImageIndex(0);
        _widgetList->append(img);



        MultiImage* imgLoading = new MultiImage(_parent);
        imgLoading->setGeometry(280,200,250,250);
        QStringList ldList = QStringList();
        ldList << ":/loading_snake.gif";
        imgLoading->setImageList(ldList);
        imgLoading->setShowImageIndex(0);
        _widgetList->append(imgLoading);

//    }

//    QLabel *loadingLabel = new  QLabel(_parent);
//    QMovie *movie = new QMovie(":/loading.gif");
//    //loadingLabel->setStyleSheet("background-color: rgba(255, 255, 255, 0);");
//    loadingLabel->setMovie(movie);
//    loadingLabel->setGeometry(280,200,250,250);
//    movie->start();

//    _widgetList->append(loadingLabel);

}

void GuiLayoutManager::SetupTestLayout()
{
    Logger::Instance()->AddLog("Creating test layout", LoggerLevel_DEBUG);

    QColor labelOffColor = QColor(Qt::white);
    QColor labelOnColor = QColor(Qt::red);

    int xMargin = _parent->width() / 50;
    int yMargin = _parent->height() / 30;
    int labelWidth = ((_parent->width() - (xMargin * 2)) / 10);
    int labelHeight = labelWidth * 0.75;

    //LABELS
    for (int i = 0; i < 10; i++)
    {
        //MixerLabelConfiguration *lConf = new MixerLabelConfiguration();

        AxelLabel *label = new AxelLabel((QWidget*)_parent);
        int labelX = xMargin + (i * labelWidth);
        QRect baseConf = QRect(labelX, yMargin, labelWidth, labelHeight);
        label->setGeometry(baseConf);
        label->setColorOff(labelOffColor);
        label->setColorOn(labelOnColor);
        label->setTextPrimary("Ch"+ QString::number(i + 1));
        if(i <= 3)
        {
            label->setTextSecondary("MIC-" + QString::number(i + 1));
        }
        else if (i == 4)
        {
            label->setTextSecondary("TELEPH");
        }
        else if (i == 5)
        {
            label->setTextSecondary("M-DISK");
        }
        else if (i == 6)
        {
            label->setTextSecondary("DJPRO1");
        }
        else if (i == 7)
        {
            label->setTextSecondary("DJPRO2");
        }
        else if (i == 8)
        {
            label->setTextSecondary("CD-1");
        }
        else if (i == 9)
        {
            label->setTextSecondary("CD-2");
        }

        label->setExtension(AxelLabel::AxelLabelExtension_TEXT);

        if(i < 9) label->setSeparators(AxelLabel::AxelLabelSeparators_RIGHT);
        //label->setId(label->textPrimary());
        label->setObjectName(label->textPrimary());
        _widgetList->append(label);

        if((i <= 4) || (i >= 8))
        {
            label = new AxelLabel((QWidget*)_parent);
            baseConf = QRect(xMargin + (i * labelWidth), _parent->height() - labelHeight - yMargin, labelWidth, labelHeight);
            label->setGeometry(baseConf);
            label->setColorOff(labelOffColor);
            label->setColorOn(labelOnColor);

            if(i < 9) label->setSeparators(AxelLabel::AxelLabelSeparators_RIGHT);

            if(i == 0)
            {
                label->setTextPrimary("CR");
                label->setImagePath(":/mic.png");
            }
            else if(i == 1)
            {
                label->setTextPrimary("ST");
                label->setImagePath(":/mic.png");
            }
            else if(i == 2)
            {
                label->setTextPrimary("TEL-1");
                label->setImagePath(":/telephone.png");
            }
            else if(i == 3)
            {
                label->setTextPrimary("TEL-2");
                label->setImagePath(":/telephone.png");
            }
            else if(i == 4)
            {
                label->setTextPrimary("BL");
                label->setImagePath(":/bluetooth.png");
            }
            else if(i == 8)
            {
                label->setTextPrimary("ST");
                label->setSeparators(AxelLabel::AxelLabelSeparators_BOTH);
                label->setImagePath(":/speaker.png");
            }
            else if(i == 9)
            {
                label->setTextPrimary("CR");
                label->setImagePath(":/speaker.png");
            }

            label->setExtension(AxelLabel::AxelLabelExtension_IMAGE);

            //label->setId(label->textPrimary());
            label->setObjectName(label->textPrimary());
            _widgetList->append(label);
        }
    }

    //CLOCK
    int bufSize = _parent->height() - (yMargin * 2) - (labelHeight * 3);
    OnairclockWidget *clock = new OnairclockWidget(((QWidget*)_parent));
    clock->resize(bufSize, bufSize);
    clock->move(xMargin * 2, yMargin * 1.5 + labelHeight);
    _widgetList->append(clock);

    //KNOB
//    KnobConfiguration *knobconfig = new KnobConfiguration();
//    knobconfig->MinValue = -10;
//    knobconfig->MaxValue = 10;
//    knobconfig->unit = "dB";
//    knobconfig->position = new WidgetPosition(new QRect(xMargin * 2, yMargin * 1.5 + labelHeight, bufSize, bufSize ));
//    QKnob *knob = new QKnob(_parent, knobconfig); // AnalogClock *clock = new AnalogClock(((QWidget*)_parent));
//    knob->setValue(4.3);
//    _widgetList->append(knob);

    //WHEEL
//    WheelConfiguration *wheelConfig = new WheelConfiguration();
//    wheelConfig->position = new WidgetPosition(new QRect(xMargin * 2, yMargin * 1.5 + labelHeight, bufSize, bufSize ));
//    for (int i = 0; i < 25; i ++)
//        wheelConfig->values<<QString::number(i);
//    QWheel *wheel = new QWheel(_parent, wheelConfig);
//    wheel->setValue(11);
//    _widgetList->append(wheel);


    //IMAGE AS LABEL
    //QPixmap pic(":/105.png");
//    QPixmap pic(":/Virgin_Radio_logo.png");
//    //QPixmap pic(":/VirginRadio.png");
//    QPixmap scaled = pic.scaled ( bufSize, bufSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );
//    QLabel *imageLabel = new QLabel(_parent);
//    int rightMargin = bufSize + xMargin * 2;
//    int imgMaxWidth =  _parent->width() - rightMargin;
//    int imageCenter = rightMargin + imgMaxWidth / 2;
//    imageLabel->move(imageCenter - scaled.width() / 2, yMargin * 2 + labelHeight );
//    imageLabel->setPixmap(scaled);
//    _widgetList->append(imageLabel);

    //LABEL TEST
//    AxelLabel *labelTest = new AxelLabel(_parent);
//    labelTest->setGeometry(250, 200, 300, 100);
//    labelTest->setColorOff(labelOffColor);
//    labelTest->setColorOn(labelOnColor);
//    labelTest->setTextPrimary("TEXT");
//    labelTest->setTextSecondary("Text2");
//    labelTest->setImage(":/bluetooth.png");
//    labelTest->setId("test");
//    labelTest->setExtension(AxelLabel::AxelLabelExtension_NONE);
//    _widgetList->append(labelTest);

//    //VUMETER
//    VuMeter *vu = new VuMeter(_parent);
//    vu->setGeometry(400,120, 50, 200);
//    vu->setMaxValue(14);
//    vu->setValue(10);
//    _widgetList->append(vu);

    //MULTIIMAGE
    MultiImage *img = new MultiImage(_parent);
    int rightMargin = bufSize + xMargin * 2;
    int imgMaxWidth =  _parent->width() - rightMargin;
    int imageCenter = rightMargin + imgMaxWidth / 2;
    img->setGeometry(imageCenter - 100 / 2, yMargin * 2 + labelHeight, 300, 250 );
    QStringList imagesList = QStringList();
    imagesList << ":/105.png" << ":/Virgin_Radio_logo.png";
    img->setImageList(imagesList);
    img->setShowImageIndex(1);
    _widgetList->append(img);

    //QPixmap pic(":/105.png");
//    QPixmap pic(":/Virgin_Radio_logo.png");
//    //QPixmap pic(":/VirginRadio.png");
//    QPixmap scaled = pic.scaled ( bufSize, bufSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );
//    QLabel *imageLabel = new QLabel(_parent);
//    int rightMargin = bufSize + xMargin * 2;
//    int imgMaxWidth =  _parent->width() - rightMargin;
//    int imageCenter = rightMargin + imgMaxWidth / 2;
//    imageLabel->move(imageCenter - scaled.width() / 2, yMargin * 2 + labelHeight );
//    imageLabel->setPixmap(scaled);
//    _widgetList->append(imageLabel);


    //GRAPH
//    GraphData *graphData = new GraphData();
//    graphData->ValueX << 1 << 2 << 7 << 20 << 100;
//    graphData->ValueY << 0 << 25 << 50 << 75 << 100;
//    QGraph *graph = new QGraph(_parent, 0);
//    graph->setGeometry(400, 150, 300, 200);
//    graph->updateData(graphData);
//    _widgetList->append(graph);


    //QT QUICK WIDGET
//    QmlWidget *qmlWidget = new QmlWidget(_parent);
//    qmlWidget->Init();
//    qmlWidget->setGeometry(400,100,300,300);
//    qmlWidget->show();
//    _widgetList->append(qmlWidget);

//    QQuickWidget *_quickWidget = new QQuickWidget(_parent);
//    //_quickWidget->setSource(QUrl(QStringLiteral("qrc:/SwitchButton.qml")));
//    _quickWidget->setSource(QUrl(QStringLiteral("qrc:/Bars.qml")));
//    _quickWidget->setGeometry(400, 100, 300, 300);
//    _quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
//    //_quickWidget->setAttribute(Qt::WA_AlwaysStackOnTop);
//    _quickWidget->setClearColor(Qt::transparent);
//    //_quickWidget->resize(320, 40);
//    //_quickWidget->showFullScreen();
//    //_quickWidget->raise();
//    _widgetList->append(_quickWidget);

//    QQuickView *view = new QQuickView();
//    QWidget *container = QWidget::createWindowContainer(view, _parent);
//    //container->setMinimumSize(200, 200);
//    //container->setMaximumSize(200, 200);
//    //container->move(imageCenter - scaled.width() / 2, yMargin * 2 + labelHeight);
//    view->setSource(QUrl("qrc:/SwitchButton.qml"));
//    container->show();
//    _widgetList->append(container);






/*    int rows = 3;
//    int cols = 4;
//    int width = _parent->width() / cols;
//    int height = _parent->height() / rows;

//    for (int i = 0; i < rows; i++)
//    {
//        if (i == 1)
//        {
//            KnobConfiguration *conf = new KnobConfiguration();
//            conf->MaxValue = 9;
//            conf->MinValue = -9;
//            conf->position = new WidgetPosition(new QRect(1*width, i * height, width, height));
//            conf->unit = "dB";
//            QKnob *knob = new QKnob((QWidget*)_parent, conf);
//            knob->setValue(5);
//            knob->setMixerId("knob");
//            knob->resize(width,height);
//            //knob->show();
//            _widgetList->append(knob);

//            WheelConfiguration *wConf = new WheelConfiguration();
//            wConf->values<<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9";
//            wConf->position = new WidgetPosition(new QRect(2 * width, i * height, width, height));
//            QWheel *wheel = new QWheel((QWidget*)_parent, wConf);
//            //wheel->show();
//            _widgetList->append(wheel);

//            MixerLabelConfiguration *lConf = new MixerLabelConfiguration();
//            lConf->colorOff = QColor(Qt::white);
//            lConf->colorOn = QColor(Qt::red);
//            lConf->textPrimary = "CR";
//            lConf->textSecondary = "MIC-1";
//            lConf->image = new QImage(":/mic.png");
//            lConf->position = new WidgetPosition(new QRect(3 * width, i * height, width, height));
//            MixerLabel *label = new MixerLabel((QWidget*)_parent, lConf);
//            label->setMixerId("Ch1");
//            //label->show();
//            _widgetList->append(label);

//        }
//        else
//        {
//            for(int j = 0; j < cols; j++)
//            {
//                AnalogClock *clock = new AnalogClock(((QWidget*)_parent));
//                clock->resize(width, height);
//                clock->move(j * width, i * height);
//                //clock->show();
//                _widgetList->append(clock);
//            }
        }
    }
*/

/*

    GraphConfiguration *configuration=new GraphConfiguration();

    configuration->position=new WidgetPosition(new QRect(0*width, 1 * height, width, height));

    GraphData *data=new GraphData();

    configuration->TicksX << 10  << 100 << 1000 << 10000 << 20000 ;
    configuration->LabelsX << "10Hz" << "100Hz" << "1Khz" << "10Khz" << "20Khz";

    configuration->TiksY << -20.0 <<0.0 <<+20.0  ;
    configuration->LabelsY << "-20.0dB" <<"0.0dB"<< "+20.0dB";

    configuration->MaxX=24000;
    configuration->MinX=10;

    configuration->MaxY=20;
    configuration->MinY=-20;

    //QGraph *graph=new QGraph((QWidget*)m_pMyWidget,configuration);

    //graph->configure(*configuration);

    QVector<double> x0(1000), y0(1000);

    int High = 10;
    int Low = -10;
    x0[0] =10;
    y0[0]=-10;
    for (int i=1; i<1000; ++i)
    {
      x0[i] = (qrand() % ((20000 + 1) - 10))+Low ;
      y0[i] = (qrand() % ((High + 1) - Low))+Low ;

    }

    data->ValueX=x0;
    data->ValueY=y0;

//  graph->updateData(*data);
//  graph->show();

*/


}

void GuiLayoutManager::NetworkManagerStatusUpdate(SystemManagerEnums::NetworkConfigurationStatus status)
{
    //return;
    if(status == SystemManagerEnums::NetworkConfigurationStatus_CONFIGURING)
        ShowMessagebox("Configuring network interface...", AxelMessagebox::MessageboxType_WAIT, 10000);
    else if(status == SystemManagerEnums::NetworkConfigurationStatus_SUCCESS)
        ShowMessagebox("Network interface configured", AxelMessagebox::MessageboxType_INFO, 4000);
    else if(status == SystemManagerEnums::NetworkConfigurationStatus_FAIL)
        ShowMessagebox("Error configuring network interface", AxelMessagebox::MessageboxType_ERROR, 4000);
}

void GuiLayoutManager::PrintUsage()
{
    qDebug() << "AxelGUI v" + _version + " Usage:\n"
             << " -h        this help\n"
             << " -v        verbose output\n"
             << " -vv       very verbose output\n"
             << " [*.json]  load layout file\n"
             << " -p [port] UDP listening port\n"
             << " -f        force desktop screen rotation\n"
             << " -d        start with demo layout\n";
}

QString GuiLayoutManager::GetVersion()
{
    return _version;
}

QString GuiLayoutManager::GetLastLayoutId()
{
    return _lastLoadedLayout;
}

QString GuiLayoutManager::GetLastLayoutJsonData()
{
    return _lastLoadedLayoutJsonData;
}

qulonglong GuiLayoutManager::GetLayoutCounter()
{
    return _layoutLoadCounter;
}

void GuiLayoutManager::ResetDebugInfo()
{
    _lastLoadedLayout = QString();
    _layoutLoadCounter = 0;
}



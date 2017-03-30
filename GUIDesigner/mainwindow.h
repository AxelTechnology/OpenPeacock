#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qudpsocket.h"
#include "ui_mainwindow.h"
#include "jsonserializer.h"
#include "abstractwidget.h"
#include "guidata.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);    
    ~MainWindow();

private slots:
    void on_btnExport_released();

    void on_cmbLayouts_currentIndexChanged(int index);

    void on_btnTest_released();

    void on_btnTestMessage_released();

    void on_btnExportAll_released();

private:
    void vuMeterTestSingle(QUdpSocket * _socket);
    void vuMeterTestArray(QUdpSocket * _socket);
    void panelTest(QUdpSocket * _socket);

    Ui::MainWindow *ui;
    QWidget* loadUiFile(QString uiFile);
    void ShowCurrentLayout(QString uiFile);
    void Initialize();
    void LoadComboLayout();
    void WriteWidgetsToFile(QString outFilePath, QWidget * parent, QList<QWidget*> * widgets, bool varsInSubdir);

    QWidget* currentLayout;
    JsonSerializer * serializer;
    bool initialized;
    bool msgTestVisible;
};

#endif // MAINWINDOW_H

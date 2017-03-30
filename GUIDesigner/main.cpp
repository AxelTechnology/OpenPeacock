#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("guidesigner");
    a.setOrganizationName("axeltechnology");
    a.setOrganizationDomain("axeltechnology.com");

    MainWindow w;
    w.show();

    return a.exec();
}

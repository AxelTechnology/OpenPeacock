#include "axelframe.h"
#include <QApplication>
#include <QtWidgets>



AxelFrame::AxelFrame(QWidget * parent, bool forceDesktopRotation)
    : QFrame(parent)
{
    Q_UNUSED(forceDesktopRotation);

    if (parent != 0)
        setGeometry(0, 0, parent->width(), parent->height());

    //Screen rotation

    QGraphicsScene *scene = new QGraphicsScene();
    view = new QGraphicsView(parent);
    scene->addWidget(this);
    view->setScene(scene);
    //view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setStyleSheet( "QGraphicsView { border-style: none; }");
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


#ifndef ON_DESKTOP
    //view->showMaximized();
    view->setGeometry(0, 0, 800, 480);
    view->showNormal();
#else
    view->setGeometry(300, 100, 800, 480);
    view->showNormal();
#endif

    //QTimer::singleShot(1000, view, SLOT(showMaximized()));

#ifdef ON_MIXER
    if(!forceDesktopRotation)
        view->rotate(180);
#endif



}


void AxelFrame::Rotate(int degrees)
{
    if (view == 0)
        return;
    view->rotate(degrees);
}


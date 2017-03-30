#ifndef QMIXERFRAME_H
#define QMIXERFRAME_H

#include <QFrame>
#include <QWidget>
#include <QtWidgets>


class AxelFrame : public QFrame
{
    QGraphicsView *view;

public:
    AxelFrame(QWidget *parent = 0, bool forceDesktopRotation = false);
    void Rotate(int degrees);

};

#endif // QMIXERFRAME_H

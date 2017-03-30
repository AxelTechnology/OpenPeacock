#ifndef KEYBOARDREADER_H
#define KEYBOARDREADER_H

#include "guilayoutmanager.h"

#include <QObject>
#include <QDebug>

class KeyboardReader : public QObject
{
    Q_OBJECT

public:
    ~KeyboardReader();
    KeyboardReader(ILayoutManager * manager);
    //void StartReading();

public slots:
    void processInput();

signals:
    void finished();

private:
    void PrintUsage();
    void PrintDebugInfo();
    void PrintDebugLayout();

    ILayoutManager *_manager;
};

#endif // KEYBOARDREADER_H

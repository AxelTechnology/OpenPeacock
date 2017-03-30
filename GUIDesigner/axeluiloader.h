#ifndef AXELUILOADER_H
#define AXELUILOADER_H

#include <QObject>
#include <QUiLoader>

class AxelUiLoader : public QUiLoader
{
public:
    AxelUiLoader(QObject * parent);

    virtual QWidget* createWidget(const QString &className, QWidget *parent = 0, const QString &name = QString());
};

#endif // AXELUILOADER_H

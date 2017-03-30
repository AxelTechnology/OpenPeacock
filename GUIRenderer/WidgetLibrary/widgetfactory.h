#ifndef WIDGETFACTORY_H
#define WIDGETFACTORY_H

#include <QObject>

class WidgetFactory
{
public:
    WidgetFactory();

    static QWidget * CreateWidgetByClassName(QString className, QWidget *parent);
    static void Initialize();
};

#endif // WIDGETFACTORY_H

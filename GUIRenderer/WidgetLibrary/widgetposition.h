#ifndef WIDGETPOSITION_H
#define WIDGETPOSITION_H

#include <QRect>

class WidgetPosition
{
public:
    WidgetPosition();

    WidgetPosition(QRect *position);

    QRect *pos;

private:


};

#endif // WIDGETPOSITION_H

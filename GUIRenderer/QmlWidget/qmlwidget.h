

#include <QObject>
#include <QWidget>

#include <QtQuickWidgets/QQuickWidget>

class QmlWidget : public QQuickWidget
{
    Q_OBJECT
public:
    explicit QmlWidget(QWidget *parent = 0);
    void Init();

signals:

public slots:
private:
    QWidget *_parent;
    QQuickWidget *_quickWidget;
};

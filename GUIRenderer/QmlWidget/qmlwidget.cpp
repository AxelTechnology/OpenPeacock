#include "qmlwidget.h"
#include <QtWidgets>
#include <QtQuickWidgets/QQuickWidget>

QmlWidget::QmlWidget(QWidget *parent) : QQuickWidget(parent)
{
    _parent = parent;
}

void QmlWidget::Init()
{

//    _quickWidget = new QQuickWidget(this);
//    _quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
//    _quickWidget->setAttribute(Qt::WA_AlwaysStackOnTop);
//    _quickWidget->setClearColor(Qt::transparent);
//    _quickWidget->resize(320, 40);
//    _quickWidget->raise();
//    _quickWidget->setSource(QUrl(QStringLiteral("qrc:/SwitchButton.qml")));
//    _quickWidget->show();

    this->setResizeMode(QQuickWidget::SizeRootObjectToView);
    this->setAttribute(Qt::WA_AlwaysStackOnTop);
    this->setClearColor(Qt::transparent);
    this->setSource(QUrl(QStringLiteral("qrc:/SwitchButton.qml")));


}

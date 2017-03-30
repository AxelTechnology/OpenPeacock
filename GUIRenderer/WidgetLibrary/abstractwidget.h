#ifndef ABSTRACTWIDGET_H
#define ABSTRACTWIDGET_H

#include <QObject>
#include <QString>
#include <QWidget>
#include <QPainter>
#include <QOpenGLWidget>
#include <QElapsedTimer>
#include <QTimer>
#include <QPropertyAnimation>


class AbstractWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool widgetVisible READ widgetVisible WRITE setWidgetVisible)
    Q_PROPERTY(int autoHideMsec READ autoHideMsec WRITE setAutoHideMsec)

    Q_PROPERTY(QString controllerDataVisible READ controllerDataVisible WRITE setControllerDataVisible)
    Q_PROPERTY(QString controllerDataEnabled READ controllerDataEnabled WRITE setControllerDataEnabled)

    Q_PROPERTY(QRect animationEndGeometry READ animationEndGeometry WRITE setAnimationEndGeometry)
    Q_PROPERTY(bool animationEnabled READ animationEnabled WRITE setAnimationEnabled)


public:        
    AbstractWidget(QWidget * parent = 0);
    ~AbstractWidget();
    virtual bool ManageRemoteCommand(QString propertyName, QJsonValue value, bool init, QString * error);
    virtual QJsonObject* GetJsonData();
    virtual QJsonObject* GetJsonDataVars();
    virtual void SetJsonData(QJsonObject* data);

    void setWidgetVisible(bool value);
    bool widgetVisible() { return _visible; }

    void setParentObjectName(QString name) { _parentObjectName = name; }
    QString parentObjectName()const { return _parentObjectName; }

    long autoHideMsec() const { return _autoHideMsec; }
    void setAutoHideMsec(int autoHideMsec) { _elapsedTimerAutoHide.restart(); _autoHideMsec = autoHideMsec; }



    QString controllerDataVisible() const { return m_controllerVisibleId; }
    void setControllerDataVisible(QString controllerVisibleId) { m_controllerVisibleId = controllerVisibleId; }

    QString controllerDataEnabled() const { return m_controllerEnabledId; }
    void setControllerDataEnabled(QString controllerEnabledId) { m_controllerEnabledId = controllerEnabledId; }



    QRect animationEndGeometry() const { return _animationEndGeometry; }
    void setAnimationEndGeometry(QRect animationEndGeometry) { _animationEndGeometry = animationEndGeometry; }

    bool animationEnabled() const { return _animationEnabled; }
    void setAnimationEnabled(bool animationEnabled) { _animationEnabled = animationEnabled; }


protected:
    QString GetStringFromColor(QColor color);
    QColor GetColorFromString(QString color);
    void PainterSetup(QPainter* painter);
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;

    void StartAnimation();
    bool _animationEnabled;
    bool _animationStarted;


private slots:
    void slotObjectNameChanged(QString);
    void timerAutoHideTick();
    void EndAnimation();
    virtual void AnimationFinished();
    virtual void AnimationStarted();


private:
    QString _parentObjectName;
    bool _visible;

    QTimer *_timerAutoHide;
    QElapsedTimer _elapsedTimerAutoHide;


    int _autoHideMsec;
    QString m_controllerVisibleId;
    QString m_controllerEnabledId;

    QPropertyAnimation * _animation;
    QTimer * _animationTimer;

    QRect _animationStartGeometry;
    QRect _animationEndGeometry;


};

#endif // ABSTRACTWIDGET_H

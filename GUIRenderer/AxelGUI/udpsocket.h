#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QObject>
#include <QUdpSocket>

class UdpSocket : public QObject
{
    Q_OBJECT

    QUdpSocket *_socket;

public:
    UdpSocket( QObject *parent);
    ~UdpSocket();

    void Start(int port);

signals:
    void dataArrived(QString);

public slots:    
    void ReadPendingDatagrams();

private:

};

#endif // UDPSOCKET_H

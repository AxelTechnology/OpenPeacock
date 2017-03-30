#include "udpsocket.h"
#include "logger.h"


UdpSocket::UdpSocket(QObject *parent=0 ) : QObject( parent )
{
}

UdpSocket::~UdpSocket()
{
}


void UdpSocket::Start(int port)
{
    _socket = new QUdpSocket(this);
    if(!_socket->bind(QHostAddress::Any, port))
        return;
    connect(_socket, SIGNAL(readyRead()), this, SLOT(ReadPendingDatagrams()));
}


void UdpSocket::ReadPendingDatagrams()
{
    while(_socket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(_socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        _socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        Logger::Instance()->AddLog("Starting data uncompression", LoggerLevel_DEBUGVERBOSE, "UDP_DATAUNCOMP");

        QString recData;
        quint32 datagramLength = datagram.length();

        if(datagramLength < 4)
            recData = datagram.data();
        else
        {
            QByteArray arr = datagram.mid(0, 4);
            QDataStream ds(arr);
            quint32 len;
            ds >> len;
            if(len != datagramLength - 4)
                recData = datagram.data();
            else
            {

            }
        }

        QByteArray uncompressedData;

//        try{
//            uncompressedData = qUncompress((unsigned char *)(datagram.data()), datagram.length());
//        }
//        catch(...){
//        }


        if (!uncompressedData.isEmpty())
            recData = uncompressedData.data();
        else
            recData = datagram.data();



        Logger::Instance()->AddLog("End data uncompression, received data: \n" + recData, LoggerLevel_DEBUGVERBOSE, "UDP_DATAUNCOMP_END");

        emit dataArrived(recData);

        Logger::Instance()->AddLog("Finished analyzing UDP Data", LoggerLevel_DEBUGVERBOSE, "UDP_DATAANALYZE_END");
    }
}

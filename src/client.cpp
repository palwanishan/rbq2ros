#include "client.h"

extern pRBCORE_SHM sharedData;

Client::Client(QObject *parent) : QObject(parent)
{
    m_host = "";
    m_port = 0;
}

Client::~Client()
{
    qInfo() << "Deconstructor";
}

void Client::connectToHost(QString host, quint16 port)
{
    if (socket->isOpen())
        disconnect();
    qInfo() << "Connecting to: " << host << " on port " << port;

    socket->connectToHost(host, port);
}

void Client::disconnect()
{
    socket->close();
}

void Client::run()
{
    // Code starts running on a thread here...
    qInfo() << Q_FUNC_INFO << QThread::currentThread();

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &Client::connected);
    connect(socket, &QTcpSocket::disconnected, this, &Client::disconnected);

    connect(socket, &QTcpSocket::stateChanged, this, &Client::stateChanged);
    connect(socket, &QTcpSocket::readyRead, this, &Client::readyRead);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Client::error);

    connectToHost(m_host, m_port);

    
    while(true){socket->waitForReadyRead();}
    // Design choices here!
    socket->waitForDisconnected(); // BLOCK
    qInfo() << "Complete!";
    deleteLater();
} // Keep the thread alive

void Client::connected()
{
    qInfo() << "Connected!";
    qInfo() << "Sending";
    // socket->write("HELLO\r\n");
}

void Client::disconnected()
{
    qInfo() << "Disconnected";
}

void Client::error(QAbstractSocket::SocketError socketError)
{
    qInfo() << "Error:" << socketError << " " << socket->errorString();
}

void Client::stateChanged(QAbstractSocket::SocketState socketState)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    qInfo() << "State: " << metaEnum.valueToKey(socketState);
}

void Client::readyRead()
{
    // qInfo() << "Data from: " << sender() << " bytes: " << socket->bytesAvailable() ;
    // qInfo() << "Data: " << socket->readAll();

    buf.append(socket->readAll());

    while (1)
    {
        bool is_header = false;
        for (uint8_t p = 0; p < buf.size() - 1; p++)
        {
            if (buf[p] == char(0xFF) && buf[p + 1] == char(0xFE))
            {
                is_header = true;
                buf.remove(0, p);
                break;
            }
        }

        const int packet_size = sizeof(ROBOT_STATE_DATA) + 4;
        if (is_header)
        {
            if (buf.size() >= packet_size)
            {
                // check tail
                if (buf[packet_size - 2] == char(0x00) && buf[packet_size - 1] == char(0x01))
                {
                    // delete header
                    buf.remove(0, 2);

                    // pairing
                    QByteArray tempBuf = buf.left(sizeof(ROBOT_STATE_DATA));
                    buf.remove(0, sizeof(ROBOT_STATE_DATA) + 2);

                    ROBOT_STATE_DATA new_robot_data;
                    memcpy(&new_robot_data, tempBuf.data(), sizeof(ROBOT_STATE_DATA));
                    sharedData->ROBOT_DATA = new_robot_data;
                }
                else
                    buf.remove(0, 2); // delete header only
            }
            else
                break;
        }
        else
        {
            buf.clear();
            break;
        }
    }
}

quint16 Client::port() const
{
    return m_port;
}

void Client::setPort(quint16 newPort)
{
    m_port = newPort;
}

const QString &Client::host() const
{
    return m_host;
}

void Client::setHost(const QString &newHost)
{
    m_host = newHost;
}
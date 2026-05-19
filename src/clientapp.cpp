#include "../include/clientapp.h"

namespace
{

constexpr quint16 SERVER_PORT = 12345;

constexpr auto SERVER_ADDRESS = "127.0.0.1";

constexpr int RECONNECT_INTERVAL_MS = 5000;

} // namespace

ClientApp::ClientApp(QObject* parent)
    : QObject(parent)
{
    connect(&_socket,
            &QTcpSocket::connected,
            this,
            &ClientApp::onConnected);

    connect(&_socket,
            &QTcpSocket::disconnected,
            this,
            &ClientApp::onDisconnected);

    connect(&_socket,
            &QTcpSocket::readyRead,
            this,
            &ClientApp::onReadyRead);

    connect(&_socket,
            &QTcpSocket::errorOccurred,
            this,
            &ClientApp::onErrorOccurred);

    _reconnect_timer.setInterval(RECONNECT_INTERVAL_MS);
    _reconnect_timer.setSingleShot(true);

    connect(&_reconnect_timer,
            &QTimer::timeout,
            this,
            &ClientApp::tryConnect);
}

void ClientApp::start()
{
    tryConnect();
}

void ClientApp::tryConnect()
{
    qInfo() << "Connecting to server: " << SERVER_ADDRESS << ":" << SERVER_PORT;

    _socket.connectToHost(SERVER_ADDRESS, SERVER_PORT);
}

void ClientApp::onConnected()
{
    qInfo() << "Connected to server: " << SERVER_ADDRESS << ":" << SERVER_PORT;
}

void ClientApp::onDisconnected()
{
    qInfo() << "Disconnected from server: " << SERVER_ADDRESS << ":" << SERVER_PORT;

    _is_connection_accepted = false;

    scheduleReconnect();
}

void ClientApp::onReadyRead()
{
    _read_buffer.append(_socket.readAll());

    while (_read_buffer.contains(protocol::TCP_PACKET_DELIMETER))
    {
        const int delimiter_index =
            _read_buffer.indexOf(protocol::TCP_PACKET_DELIMETER);

        const QByteArray message =
            _read_buffer.left(delimiter_index);

        _read_buffer.remove(0, delimiter_index + 1);

        processMessage(message);
    }
}

void ClientApp::processMessage(const QByteArray& message)
{
    QJsonObject object;

    if (!protocol::Deserialize(message, &object))
    {
        qInfo() << "Failed to parse JSON message";

        return;
    }

    handleJsonMessage(object);
}

void ClientApp::handleJsonMessage(const QJsonObject& object)
{
    const QString type =
        object[protocol::kType].toString();

    if (type == protocol::kAck)
    {
        _is_connection_accepted = true;

        qInfo() << "Connection ACK received";

        return;
    }

    qInfo() << "Unknown message type: " << type;
}

void ClientApp::onErrorOccurred(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);

    qInfo() << "Socket error: " << _socket.errorString();

    scheduleReconnect();
}

void ClientApp::scheduleReconnect()
{
    if (_reconnect_timer.isActive())
    {
        return;
    }

    qInfo() << "Reconnect scheduled in "
             << RECONNECT_INTERVAL_MS
             << " ms";

    _reconnect_timer.start();
}

#include "../include/tcpclient.h"

namespace
{

constexpr quint16 SERVER_PORT = 12345;

constexpr auto SERVER_ADDRESS = "127.0.0.1";

constexpr int RECONNECT_INTERVAL_MS = 5000;

} // namespace

TcpClient::TcpClient(QObject* parent)
    : QObject(parent)
{
    connect(&_socket,
            &QTcpSocket::connected,
            this,
            &TcpClient::onConnected);

    connect(&_socket,
            &QTcpSocket::disconnected,
            this,
            &TcpClient::onDisconnected);

    connect(&_socket,
            &QTcpSocket::readyRead,
            this,
            &TcpClient::onReadyRead);

    connect(&_socket,
            &QTcpSocket::errorOccurred,
            this,
            &TcpClient::onErrorOccurred);

    _reconnect_timer.setInterval(RECONNECT_INTERVAL_MS);
    _reconnect_timer.setSingleShot(true);

    connect(&_reconnect_timer,
            &QTimer::timeout,
            this,
            &TcpClient::tryConnect);
}

void TcpClient::start()
{
    tryConnect();
}

void TcpClient::sendData(
    const QJsonObject& json)
{
    if (_socket.state() != QAbstractSocket::ConnectedState)
    {
        return;
    }

    auto data =
        protocol::Serialize(json);

    _socket.write(data);
}

void TcpClient::tryConnect()
{
    qInfo() << "Connecting to server: "
            << SERVER_ADDRESS << ":" << SERVER_PORT;

    _socket.connectToHost(SERVER_ADDRESS, SERVER_PORT);
}

void TcpClient::onConnected()
{
    qInfo() << "Connected to server: "
            << SERVER_ADDRESS << ":" << SERVER_PORT;
}

void TcpClient::onDisconnected()
{
    qInfo() << "Disconnected from server: "
            << SERVER_ADDRESS << ":" << SERVER_PORT;

    scheduleReconnect();
}

void TcpClient::onReadyRead()
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

void TcpClient::processMessage(
    const QByteArray& message)
{
    QJsonObject json;

    if (!protocol::Deserialize(message, &json))
    {
        qInfo() << "Failed to parse JSON message";

        return;
    }

    handleJsonMessage(json);
}

void TcpClient::handleJsonMessage(
    const QJsonObject& json)
{
    const QString type =
        json[protocol::kType].toString();

    if (type == protocol::kAck)
    {
        qInfo() << "Connection ACK received";

        return;
    }
    else if (type ==
               protocol::kStartTransmission)
    {
        qInfo() << "Start command received";

        emit startCommandReceived();

        return;
    }
    else if (type ==
               protocol::kStopTransmission)
    {
        qInfo() << "Stop command received";

        emit stopCommandReceived();

        return;
    }
    else if (type ==
               protocol::kLimitsConfig)
    {
        qInfo() << "Limits configuration received";

        handleLimitsConfigMessage(json);

        return;
    }

    qInfo() << "Unknown message type: " << type;
}

void TcpClient::handleLimitsConfigMessage(
    const QJsonObject& json)
{
    sharedTypes::LimitsConfig config;

    QJsonValue val =
        json.value(protocol::kLatency);

    if (!val.isUndefined() &&
        !val.isNull())
    {
        config.latency = val.toDouble();
    }

    val = json.value(protocol::kErrors);

    if (!val.isUndefined() &&
        !val.isNull())
    {
        config.errors = val.toInt();
    }

    val = json.value(protocol::kCpuUsage);

    if (!val.isUndefined() &&
        !val.isNull())
    {
        config.cpu_usage = val.toInt();
    }

    val = json.value(protocol::kTemperature);

    if (!val.isUndefined() &&
        !val.isNull())
    {
        config.temperature = val.toDouble();
    }

    emit limitsConfigReceived(config);
}

void TcpClient::onErrorOccurred(
    QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);

    qInfo() << "Socket error: " << _socket.errorString();

    scheduleReconnect();
}

void TcpClient::scheduleReconnect()
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

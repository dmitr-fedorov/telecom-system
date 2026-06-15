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
    if (_socket.state() !=
        QAbstractSocket::ConnectedState)
    {
        qWarning()
        << "Ошибка: попытка отправить данные "
           "без подключения к серверу";

        return;
    }

    const auto data =
        protocol::Serialize(json);

    const qint64 written =
        _socket.write(data);

    if (written == -1)
    {
        qWarning()
        << "Ошибка: не удалось отправить данные:"
        << _socket.errorString();

        return;
    }
}

void TcpClient::tryConnect()
{
    if (_socket.state() !=
        QAbstractSocket::UnconnectedState)
    {
        return;
    }

    qInfo().nospace()
        << "Подключение к серверу: "
        << SERVER_ADDRESS
        << ':'
        << SERVER_PORT
        << "...";

    _socket.connectToHost(SERVER_ADDRESS, SERVER_PORT);
}

void TcpClient::onConnected()
{
    qInfo().nospace()
        << "Подключено к серверу: "
        << SERVER_ADDRESS
        << ':'
        << SERVER_PORT;
}

void TcpClient::onDisconnected()
{
    qInfo().nospace()
        << "Отключено от сервера: "
        << SERVER_ADDRESS
        << ':'
        << SERVER_PORT;

    scheduleReconnect();
}

void TcpClient::onReadyRead()
{
    _read_buffer.append(_socket.readAll());

    while (
        _read_buffer
            .contains(protocol::TCP_PACKET_DELIMETER))
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

    if (!protocol::Deserialize(
            message,
            &json))
    {
        qWarning()
            << "Ошибка: не удалось разобрать "
               "JSON сообщение от сервера";

        return;
    }

    handleJsonMessage(json);
}

void TcpClient::handleJsonMessage(
    const QJsonObject& json)
{
    const QString type =
        json[protocol::kType].toString();

    if (type.isEmpty())
    {
        qWarning()
            << "Ошибка: получено JSON "
               "сообщение от сервера "
               "без валидного поля type";

        return;
    }

    if (type == protocol::kAck)
    {
        qInfo()
            << "Получено подтверждение соединения";

        return;
    }

    if (type ==
        protocol::kStartTransmission)
    {
        qInfo()
            << "Получена команда на начало "
               "отправки данных";

        emit startCommandReceived();

        return;
    }

    if (type ==
        protocol::kStopTransmission)
    {
        qInfo()
            << "Получена команда на прекращение "
               "отправки данных";

        emit stopCommandReceived();

        return;
    }

    if (type ==
        protocol::kLimitsConfig)
    {
        qInfo() << "Получена конфигурация лимитов";

        handleLimitsConfigMessage(json);

        return;
    }

    qWarning()
        << "Ошибка: получено сообщение "
           "неизвестного типа:" << type;
}

void TcpClient::handleLimitsConfigMessage(
    const QJsonObject& json)
{
    sharedTypes::LimitsConfig config;

    QJsonValue val =
        json.value(protocol::kLatency);

    if (!val.isUndefined() &&
        !val.isNull() &&
        val.isDouble())
    {
        config.latency =
            val.toDouble();
    }

    val =
        json.value(protocol::kErrors);

    if (!val.isUndefined() &&
        !val.isNull() &&
        val.isDouble())
    {
        config.errors =
            val.toInt();
    }

    val =
        json.value(protocol::kCpuUsage);

    if (!val.isUndefined() &&
        !val.isNull() &&
        val.isDouble())
    {
        config.cpu_usage =
            val.toInt();
    }

    val =
        json.value(protocol::kTemperature);

    if (!val.isUndefined() &&
        !val.isNull() &&
        val.isDouble())
    {
        config.temperature =
            val.toDouble();
    }

    emit limitsConfigReceived(config);
}

void TcpClient::onErrorOccurred(
    QAbstractSocket::SocketError)
{
    qWarning()
        << "Ошибка сокета:"
        << _socket.errorString();

    if (_socket.state() !=
        QAbstractSocket::ConnectedState)
    {
        scheduleReconnect();
    }
}

void TcpClient::scheduleReconnect()
{
    if (_reconnect_timer.isActive())
    {
        return;
    }

    qInfo() << "Повторное соединение "
               "запланировано через"
             << RECONNECT_INTERVAL_MS
             << "мс";

    _reconnect_timer.start();
}

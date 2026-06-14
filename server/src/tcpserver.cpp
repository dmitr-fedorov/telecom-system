#include "../include/tcpserver.h"

TcpServer::TcpServer(
    QObject* parent)
    : QObject(parent)
{
}

void TcpServer::startServer()
{
    if (_server != nullptr)
    {
        return;
    }

    _server = new QTcpServer(this);

    connect(_server,
            &QTcpServer::newConnection,
            this,
            &TcpServer::onNewConnection);

    if (!_server->listen(QHostAddress::Any, _SERVER_PORT))
    {
        emit eventOccurred(
            QString("Ошибка: Не удалось запустить сервер: %1")
                .arg(_server->errorString()));

        delete _server;
        _server = nullptr;

        return;
    }

    emit serverStarted();

    emit eventOccurred(
        QString("Сервер запущен на порте %1")
            .arg(_SERVER_PORT));
}

void TcpServer::stopServer()
{
    if (_server != nullptr)
    {
        _server->close();
        _server->deleteLater();
        _server = nullptr;
    }

    for (auto it = _clients.begin();
         it != _clients.end(); ++it)
    {
        auto* socket = it.key();

        socket->disconnectFromHost();
        socket->disconnect(this);
        socket->deleteLater();
    }

    _clients.clear();

    _is_clients_running = false;

    _id_counter = 0;

    emit eventOccurred("Сервер остановлен");
}

void TcpServer::startClients()
{
    if (_is_clients_running)
    {
        emit eventOccurred(
            "Ошибка: Передача данных клиентами уже начата");

        return;
    }

    QJsonObject object;
    object[protocol::kType] =
        protocol::kStartTransmission;

    broadcastMessage(object);

    _is_clients_running = true;

    emit eventOccurred(
        "Передача данных клиентами активна");

    emit clientsRunningStateChanged(
        _is_clients_running);
}

void TcpServer::stopClients()
{
    if (!_is_clients_running)
    {
        emit eventOccurred(
            "Ошибка: Передача данных клиентами уже остановлена");

        return;
    }

    QJsonObject object;
    object[protocol::kType] =
        protocol::kStopTransmission;

    broadcastMessage(object);

    _is_clients_running = false;

    emit eventOccurred(
        "Передача данных клиентами остановлена");

    emit clientsRunningStateChanged(
        _is_clients_running);
}

void TcpServer::applyLimitsConfig(
    const sharedTypes::LimitsConfig& config)
{
    _last_limits_config = config;

    auto configJson =
        toJson(config);

    broadcastMessage(configJson);

    emit eventOccurred(
            "Конфигурация лимитов применена");
}

void TcpServer::onNewConnection()
{
    while (_server->hasPendingConnections())
    {
        auto* socket =
            _server->nextPendingConnection();

        const auto client_id =
            getNewClientId();

        _clients[socket].client_id = client_id;

        connect(socket,
                &QTcpSocket::readyRead,
                this,
                &TcpServer::onReadyRead);

        connect(socket,
                &QTcpSocket::disconnected,
                this,
                &TcpServer::onClientDisconnected);

        connect(socket,
                &QTcpSocket::errorOccurred,
                this,
                &TcpServer::onSocketError);

        emit clientConnectionStateChanged(client_id,
                                socket->peerAddress().toString(),
                                "Подключен");

        emit eventOccurred(
            QString("Клиент %1 подключился")
                .arg(client_id));

        sendAck(socket, client_id);

        sendLastLimitsConfig(socket, client_id);

        if (_is_clients_running)
        {
            sendStartCommand(socket, client_id);
        }
    }
}

void TcpServer::onClientDisconnected()
{
    auto* socket =
        qobject_cast<QTcpSocket*>(sender());

    if (socket == nullptr)
    {
        return;
    }

    auto it = _clients.find(socket);

    if (it == _clients.end())
    {
        return;
    }

    emit clientConnectionStateChanged(it->client_id,
                            socket->peerAddress().toString(),
                            "Отключен");

    emit eventOccurred(
        QString("Клиент %1 отключился")
            .arg(it->client_id));

    _clients.remove(socket);

    socket->disconnect(this);
    socket->deleteLater();
}

void TcpServer::onReadyRead()
{
    auto* socket =
        qobject_cast<QTcpSocket*>(sender());

    if (socket == nullptr)
    {
        return;
    }

    auto it = _clients.find(socket);

    if (it == _clients.end())
    {
        return;
    }

    auto& buffer = it->buffer;

    buffer.append(socket->readAll());

    while (true)
    {
        const int delimiter_index =
            buffer.indexOf(
            protocol::TCP_PACKET_DELIMETER);

        if (delimiter_index == -1)
        {
            break;
        }

        const auto message =
            buffer.left(delimiter_index);

        buffer.remove(
            0,
            delimiter_index + 1);

        processMessage(it->client_id, message);
    }
}

QString TcpServer::getNewClientId()
{
    return QString("client_%1")
    .arg(++_id_counter);
}

bool TcpServer::sendMessage(
    QTcpSocket* socket,
    const QJsonObject& object)
{
    if (socket == nullptr)
    {
        emit eventOccurred(
            "Ошибка: попытка отправить сообщение в null socket");

        return false;
    }

    auto it = _clients.find(socket);

    if (it == _clients.end())
    {
        emit eventOccurred(
            "Ошибка: сокет отсутствует в списке клиентов");

        return false;
    }

    if (socket->state() !=
        QAbstractSocket::ConnectedState)
    {
        emit eventOccurred(
            QString("Ошибка: клиент %1 не подключен")
                .arg(it->client_id));

        return false;
    }

    const auto data =
        protocol::Serialize(object);

    const qint64 written =
        socket->write(data);

    if (written == -1)
    {
        emit eventOccurred(
            QString("Ошибка отправки клиенту %1: %2")
                .arg(it->client_id,
                     socket->errorString()));

        return false;
    }

    return true;
}

bool TcpServer::broadcastMessage(
    const QJsonObject& object)
{
    bool success = false;

    for (auto it = _clients.begin();
         it != _clients.end(); ++it)
    {
        if (sendMessage(it.key(), object))
        {
            success = true;
        }
    }

    if (!success)
    {
        emit eventOccurred(
            "Не удалось отправить сообщение ни одному клиенту");
    }

    return success;
}

void TcpServer::sendAck(
    QTcpSocket* socket,
    const QString& clientId)
{
    QJsonObject object;

    object[protocol::kType] =
        protocol::kAck;

    if (sendMessage(socket, object))
    {
        emit eventOccurred(
            QString("Connection Ack отправлен клиенту %1")
                .arg(clientId));
    }
    else
    {
        emit eventOccurred(
            QString("Не удалось отправить "
                    "Connection Ack клиенту %1")
                .arg(clientId));
    }
}

void TcpServer::sendStartCommand(
    QTcpSocket* socket,
    const QString& clientId)
{
    QJsonObject object;
    object[protocol::kType] =
        protocol::kStartTransmission;

    if (sendMessage(socket, object))
    {
        emit eventOccurred(
            QString("Передача данных клиентом %1 начата")
                .arg(clientId));
    }
    else
    {
        emit eventOccurred(
            QString("Не удалось начать отправку "
                    "данных клиентом %1")
                .arg(clientId));
    }
}

void TcpServer::sendLastLimitsConfig(
    QTcpSocket* socket,
    const QString& clientId)
{
    auto configJson =
        toJson(_last_limits_config);

    if (sendMessage(socket, configJson))
    {
        emit eventOccurred(
            QString("Конфигурация лимитов "
                    "отправлена клиенту %1")
                .arg(clientId));
    }
    else
    {
        emit eventOccurred(
            QString("Не удалось отправить "
                    "конфигурацию лимитов клиенту %1")
                .arg(clientId));
    }
}

QJsonObject TcpServer::toJson(
    const sharedTypes::LimitsConfig& config)
{
    QJsonObject result;
    result[protocol::kType] =
        protocol::kLimitsConfig;

    if (config.latency.has_value())
    {
        result[protocol::kLatency] =
            config.latency.value();
    }

    if (config.errors.has_value())
    {
        result[protocol::kErrors] =
            config.errors.value();
    }

    if (config.cpu_usage.has_value())
    {
        result[protocol::kCpuUsage] =
            config.cpu_usage.value();
    }

    if (config.temperature.has_value())
    {
        result[protocol::kTemperature] =
            config.temperature.value();
    }

    return result;
}

void TcpServer::processMessage(
    const QString& clientId,
    const QByteArray& message)
{
    QJsonObject object;

    if (!protocol::Deserialize(
            message,
            &object))
    {
        emit eventOccurred(
            QString(
                "Ошибка: не удалось разобрать "
                "JSON сообщение от клиента %1").arg(clientId));

        return;
    }

    const auto type =
        object.take(protocol::kType).toString();

    if (type.isEmpty())
    {
        emit eventOccurred(
            QString(
                "Ошибка: клиент %1 прислал JSON "
                "без поля type").arg(clientId));

        return;
    }

    const auto content =
        formatContent(type, object);

    emit clientDataReceived(
        clientId,
        type,
        content,
        QDateTime::currentDateTime());
}

QString TcpServer::formatContent(
    const QString& type,
    const QJsonObject& object)
{
    if (type == protocol::kLog)
    {
        return QString("[%1] %2")
        .arg(object.value(protocol::kSeverity).toString(),
             object.value(protocol::kMessage).toString());
    }

    QStringList parts;

    for (auto it = object.begin();
         it != object.end();
         ++it)
    {
        parts.append(
            QString("%1=%2")
                .arg(it.key(),
                     it.value().toVariant().toString()));
    }

    return parts.join(", ");
}

void TcpServer::onSocketError(
    QAbstractSocket::SocketError)
{
    auto* socket =
        qobject_cast<QTcpSocket*>(sender());

    if (socket == nullptr)
    {
        return;
    }

    auto it = _clients.find(socket);

    if (it == _clients.end())
    {
        return;
    }

    emit eventOccurred(
        QString("Ошибка клиента %1: %2")
            .arg(it->client_id,
                 socket->errorString()));
}

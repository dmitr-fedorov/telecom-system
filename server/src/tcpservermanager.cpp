#include "../include/tcpservermanager.h"

TcpServerManager::TcpServerManager(
    QObject* parent)
    : QObject(parent)
{
}

void TcpServerManager::startServer()
{
    _server = new QTcpServer(this);

    connect(_server,
            &QTcpServer::newConnection,
            this,
            &TcpServerManager::onNewConnection);

    if (!_server->listen(QHostAddress::Any, _SERVER_PORT))
    {
        emit eventOccurred(
            QString("Failed to start server: %1")
                .arg(_server->errorString()));
    }

    emit eventOccurred(
        QString("Server started on port %1")
            .arg(_SERVER_PORT));
}

void TcpServerManager::stopServer()
{
    if (_server != nullptr)
    {
        _server->close();
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

    emit eventOccurred("Server stopped");
}

void TcpServerManager::startClients()
{
    if (_is_clients_running)
    {
        emit eventOccurred(
            "Clients already started");

        return;
    }

    QJsonObject object;
    object["type"] = "Start";

    const bool success =
        broadcastMessage(object);

    if (!success)
    {
        emit eventOccurred(
            "No active clients");
    }

    _is_clients_running = true;

    emit eventOccurred(
        "Clients started");

    emit clientsRunningStateChanged(
        _is_clients_running);
}

void TcpServerManager::stopClients()
{
    if (!_is_clients_running)
    {
        emit eventOccurred(
            "Clients already stopped");

        return;
    }

    QJsonObject object;
    object["type"] = "Stop";

    const bool success =
        broadcastMessage(object);

    if (!success)
    {
        emit eventOccurred(
            "No active clients");
    }

    _is_clients_running = false;

    emit eventOccurred(
        "Clients stopped");

    emit clientsRunningStateChanged(
        _is_clients_running);
}

void TcpServerManager::applyConfiguration(
    int limit_value)
{
    QJsonObject object;
    object["type"] = "Config";
    object["limit"] = limit_value;

    broadcastMessage(object);

    emit eventOccurred(
        QString("Configuration applied. "
                "Limit value = %1")
            .arg(limit_value));
}

void TcpServerManager::onNewConnection()
{
    while (_server->hasPendingConnections())
    {
        auto* socket =
            _server->nextPendingConnection();

        const auto client_id =
            getNewClientId();

        ClientContext context;
        context.client_id = client_id;

        _clients[socket] = context;

        connect(socket,
                &QTcpSocket::readyRead,
                this,
                &TcpServerManager::onReadyRead);

        connect(socket,
                &QTcpSocket::disconnected,
                this,
                &TcpServerManager::onClientDisconnected);

        ClientInfo info;
        info.client_id = client_id;
        info.ip_address =
            socket->peerAddress().toString();
        info.port = socket->peerPort();
        info.status = "Connected";

        emit clientConnected(info);

        sendAck(socket);

        if (_is_clients_running)
        {
            QJsonObject object;
            object["type"] = "Start";

            sendMessage(socket, object);
        }
    }
}

void TcpServerManager::onClientDisconnected()
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

    emit clientDisconnected(it->client_id);

    emit eventOccurred(
        QString("Client disconnected: %1")
            .arg(it->client_id));

    _clients.remove(socket);

    socket->disconnect(this);
    socket->deleteLater();
}

void TcpServerManager::onReadyRead()
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

QString TcpServerManager::getNewClientId()
{
    return QString("client_%1")
    .arg(++_id_counter);
}

void TcpServerManager::sendMessage(
    QTcpSocket* socket,
    const QJsonObject& object)
{
    const auto data =
        protocol::Serialize(object);

    if (socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write(data);
    }
}

bool TcpServerManager::broadcastMessage(
    const QJsonObject& object)
{
    bool success = false;

    const auto data =
        protocol::Serialize(object);

    for (auto it = _clients.begin();
         it != _clients.end(); ++it)
    {
        auto* socket = it.key();

        if (socket->state() !=
            QAbstractSocket::ConnectedState)
        {
            continue;
        }

        const auto written =
            socket->write(data);

        if (written != -1)
        {
            success = true;
        }
    }

    return success;
}

void TcpServerManager::sendAck(QTcpSocket* socket)
{
    const QJsonObject object =
        protocol::CreateAckMessage();

    sendMessage(socket, object);
}

void TcpServerManager::processMessage(
    const QString& clientId,
    const QByteArray& message)
{
    QJsonObject object;

    if (!protocol::Deserialize(
            message,
            &object))
    {
        emit eventOccurred(
            "Failed to parse JSON message");

        return;
    }

    const auto type =
        object.take(protocol::kType).toString();

    const auto content =
        QString::fromUtf8(
            QJsonDocument(object)
                .toJson(QJsonDocument::Compact));

    emit clientDataReceived(
        clientId,
        type,
        content,
        QDateTime::currentDateTime());
}

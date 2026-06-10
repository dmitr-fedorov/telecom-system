#include "../include/tcpserver.h"

TcpServer::TcpServer(
    QObject* parent)
    : QObject(parent)
{
}

void TcpServer::startServer()
{
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
    }

    emit eventOccurred(
        QString("Сервер запущен на порте %1")
            .arg(_SERVER_PORT));
}

void TcpServer::stopServer()
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
    object[protocol::kType] = "Start";

    const bool success =
        broadcastMessage(object);

    if (!success)
    {
        emit eventOccurred(
            "Нет подключенных клиентов");
    }

    _is_clients_running = true;

    emit eventOccurred(
        "Начата передача данных клиентами");

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
    object[protocol::kType] = "Stop";

    const bool success =
        broadcastMessage(object);

    if (!success)
    {
        emit eventOccurred(
            "Нет подключенных клиентов");
    }

    _is_clients_running = false;

    emit eventOccurred(
        "Передача данных клиентами остановлена");

    emit clientsRunningStateChanged(
        _is_clients_running);
}

void TcpServer::applyConfiguration(
    int limit_value)
{
    QJsonObject object;
    object[protocol::kType] = "Config";
    object["limit"] = limit_value;

    const bool success =
        broadcastMessage(object);

    if (!success)
    {
        emit eventOccurred(
            "Нет подключенных клиентов");

        return;
    }

    emit eventOccurred(
        "Конфигурация лимитов отправлена клиентам");
}

void TcpServer::onNewConnection()
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
                &TcpServer::onReadyRead);

        connect(socket,
                &QTcpSocket::disconnected,
                this,
                &TcpServer::onClientDisconnected);

        emit clientConnectionStateChanged(client_id,
                                socket->peerAddress().toString(),
                                "Подключен");

        emit eventOccurred(
            QString("Клиент подключился: %1")
                .arg(client_id));

        sendAck(socket);

        if (_is_clients_running)
        {
            QJsonObject object;
            object[protocol::kType] = "Start";

            sendMessage(socket, object);
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
        QString("Клиент отключился: %1")
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

void TcpServer::sendMessage(
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

bool TcpServer::broadcastMessage(
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

void TcpServer::sendAck(QTcpSocket* socket)
{
    QJsonObject object;

    object[protocol::kType] = protocol::kAck;

    sendMessage(socket, object);
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
                "JSON сообщение от клиента: ").arg(clientId));

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

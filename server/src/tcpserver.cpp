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
    object[protocol::kType] = "Start";

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
    object[protocol::kType] = "Stop";

    broadcastMessage(object);

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

    if (success)
    {
        emit eventOccurred(
            "Конфигурация лимитов отправлена клиентам");
    }
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

        if (_is_clients_running)
        {
            QJsonObject object;
            object[protocol::kType] = "Start";

            if (sendMessage(socket, object))
            {
                emit eventOccurred(
                    QString("Передача данных клиентом %1 начата")
                        .arg(client_id));
            }
            else
            {
                emit eventOccurred(
                    QString("Не удалось начать отправку "
                            "данных клиентом %1")
                        .arg(client_id));
            }

            // ### ОТПРАВИТЬ КОНФИГУРАЦИЮ ЛИМИТОВ
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
        QString::fromUtf8(
            QJsonDocument(object)
                .toJson(QJsonDocument::Compact));

    emit clientDataReceived(
        clientId,
        type,
        content,
        QDateTime::currentDateTime());
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

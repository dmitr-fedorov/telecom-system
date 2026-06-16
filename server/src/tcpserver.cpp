#include "../include/tcpserver.h"

namespace server {

TcpServer::TcpServer(QObject* parent) : QObject(parent) {}

void TcpServer::startServer() {
  if (server_ != nullptr) {
    return;
  }

  server_ = new QTcpServer(this);

  connect(server_, &QTcpServer::newConnection, this,
          &TcpServer::onNewConnection);

  if (!server_->listen(QHostAddress::Any, server_port_)) {
    emit eventOccurred(QString("Ошибка: Не удалось запустить сервер: %1")
                           .arg(server_->errorString()));

    delete server_;
    server_ = nullptr;

    return;
  }

  emit serverStarted();

  emit eventOccurred(QString("Сервер запущен на порте %1").arg(server_port_));
}

void TcpServer::stopServer() {
  if (server_ != nullptr) {
    server_->close();
    server_->deleteLater();
    server_ = nullptr;
  }

  for (auto it = clients_.begin(); it != clients_.end(); ++it) {
    auto* socket = it.key();

    socket->disconnectFromHost();
    socket->disconnect(this);
    socket->deleteLater();
  }

  clients_.clear();

  is_clients_running_ = false;

  id_counter_ = 0;

  emit eventOccurred("Сервер остановлен");
}

void TcpServer::startClientsDataTransmission() {
  if (is_clients_running_) {
    emit eventOccurred("Ошибка: Передача данных клиентами уже начата");

    return;
  }

  QJsonObject json;
  json[shared::protocol::k_type] = shared::protocol::k_start_transmission;

  broadcastMessage(json);

  is_clients_running_ = true;

  emit eventOccurred("Передача данных клиентами активна");

  emit clientsTransmittingStateChanged(is_clients_running_);
}

void TcpServer::stopClientsDataTransmission() {
  if (!is_clients_running_) {
    emit eventOccurred("Ошибка: Передача данных клиентами уже остановлена");

    return;
  }

  QJsonObject json;
  json[shared::protocol::k_type] = shared::protocol::k_stop_transmission;

  broadcastMessage(json);

  is_clients_running_ = false;

  emit eventOccurred("Передача данных клиентами остановлена");

  emit clientsTransmittingStateChanged(is_clients_running_);
}

void TcpServer::applyLimitsConfig(const shared::types::LimitsConfig& config) {
  last_limits_config_ = config;

  auto configJson = toJson(config);

  broadcastMessage(configJson);

  emit eventOccurred("Конфигурация лимитов применена");
}

void TcpServer::onNewConnection() {
  while (server_->hasPendingConnections()) {
    auto* socket = server_->nextPendingConnection();

    const auto client_id = getNewClientId();

    clients_[socket].client_id = client_id;

    connect(socket, &QTcpSocket::readyRead, this, &TcpServer::onReadyRead);

    connect(socket, &QTcpSocket::disconnected, this,
            &TcpServer::onClientDisconnected);

    connect(socket, &QTcpSocket::errorOccurred, this,
            &TcpServer::onSocketError);

    server::types::ClientInfo info;
    info.client_id = client_id;
    info.ip_address = socket->peerAddress().toString();
    info.state = "Подключен";

    emit clientConnectionStateChanged(info);

    emit eventOccurred(QString("Клиент %1 подключился").arg(client_id));

    sendAck(socket, client_id);

    sendLastLimitsConfig(socket, client_id);

    if (is_clients_running_) {
      sendStartCommand(socket, client_id);
    }
  }
}

void TcpServer::onClientDisconnected() {
  auto* socket = qobject_cast<QTcpSocket*>(sender());

  if (socket == nullptr) {
    return;
  }

  auto it = clients_.find(socket);

  if (it == clients_.end()) {
    return;
  }

  server::types::ClientInfo info;
  info.client_id = it->client_id;
  info.ip_address = socket->peerAddress().toString();
  info.state = "Отключен";

  emit clientConnectionStateChanged(info);

  emit eventOccurred(QString("Клиент %1 отключился").arg(it->client_id));

  clients_.remove(socket);

  socket->disconnect(this);
  socket->deleteLater();
}

void TcpServer::onReadyRead() {
  auto* socket = qobject_cast<QTcpSocket*>(sender());

  if (socket == nullptr) {
    return;
  }

  auto it = clients_.find(socket);

  if (it == clients_.end()) {
    return;
  }

  auto& buffer = it->buffer;

  buffer.append(socket->readAll());

  while (true) {
    const int delimiter_index =
        buffer.indexOf(shared::protocol::tcp_packet_delimeter);

    if (delimiter_index == -1) {
      break;
    }

    const auto message = buffer.left(delimiter_index);

    buffer.remove(0, delimiter_index + 1);

    processMessage(message, it->client_id);
  }
}

void TcpServer::onSocketError(QAbstractSocket::SocketError error) {
  Q_UNUSED(error);

  auto* socket = qobject_cast<QTcpSocket*>(sender());

  if (socket == nullptr) {
    return;
  }

  auto it = clients_.find(socket);

  if (it == clients_.end()) {
    return;
  }

  emit eventOccurred(QString("Ошибка клиента %1: %2")
                         .arg(it->client_id, socket->errorString()));
}

QString TcpServer::getNewClientId() {
  return QString("client_%1").arg(++id_counter_);
}

bool TcpServer::sendMessage(QTcpSocket* socket, const QJsonObject& json) {
  if (socket == nullptr) {
    emit eventOccurred("Ошибка: попытка отправить сообщение в null socket");

    return false;
  }

  auto it = clients_.find(socket);

  if (it == clients_.end()) {
    emit eventOccurred("Ошибка: сокет отсутствует в списке клиентов");

    return false;
  }

  if (socket->state() != QAbstractSocket::ConnectedState) {
    emit eventOccurred(
        QString("Ошибка: клиент %1 не подключен").arg(it->client_id));

    return false;
  }

  const auto data = shared::protocol::Serialize(json);

  const qint64 written = socket->write(data);

  if (written == -1) {
    emit eventOccurred(QString("Ошибка отправки клиенту %1: %2")
                           .arg(it->client_id, socket->errorString()));

    return false;
  }

  return true;
}

bool TcpServer::broadcastMessage(const QJsonObject& json) {
  bool success = false;

  for (auto it = clients_.begin(); it != clients_.end(); ++it) {
    if (sendMessage(it.key(), json)) {
      success = true;
    }
  }

  if (!success) {
    emit eventOccurred("Не удалось отправить сообщение ни одному клиенту");
  }

  return success;
}

void TcpServer::sendAck(QTcpSocket* socket, const QString& client_id) {
  QJsonObject json;
  json[shared::protocol::k_type] = shared::protocol::k_ack;
  json[shared::protocol::k_client_id] = client_id;

  if (!sendMessage(socket, json)) {
    emit eventOccurred(QString("Не удалось отправить "
                               "Connection Ack клиенту %1")
                           .arg(client_id));

    return;
  }

  emit eventOccurred(
      QString("Connection Ack отправлен клиенту %1").arg(client_id));
}

void TcpServer::sendStartCommand(QTcpSocket* socket, const QString& client_id) {
  QJsonObject json;
  json[shared::protocol::k_type] = shared::protocol::k_start_transmission;

  if (!sendMessage(socket, json)) {
    emit eventOccurred(QString("Не удалось отправить команду "
                               "на начало отправки данных клиенту %1")
                           .arg(client_id));
  }

  emit eventOccurred(
      QString("Команда на начало отправки данных отправлена клиенту %1")
          .arg(client_id));
}

void TcpServer::sendLastLimitsConfig(QTcpSocket* socket,
                                     const QString& client_id) {
  auto json = toJson(last_limits_config_);

  if (!sendMessage(socket, json)) {
    emit eventOccurred(QString("Не удалось отправить "
                               "конфигурацию лимитов клиенту %1")
                           .arg(client_id));

    return;
  }

  emit eventOccurred(QString("Конфигурация лимитов "
                             "отправлена клиенту %1")
                         .arg(client_id));
}

QJsonObject TcpServer::toJson(const shared::types::LimitsConfig& config) {
  QJsonObject result;
  result[shared::protocol::k_type] = shared::protocol::k_limits_config;

  if (config.latency.has_value()) {
    result[shared::protocol::k_latency] = config.latency.value();
  }

  if (config.errors.has_value()) {
    result[shared::protocol::k_errors] = config.errors.value();
  }

  if (config.cpu_usage.has_value()) {
    result[shared::protocol::k_cpu_usage] = config.cpu_usage.value();
  }

  if (config.temperature.has_value()) {
    result[shared::protocol::k_temperature] = config.temperature.value();
  }

  return result;
}

void TcpServer::processMessage(const QByteArray& message,
                               const QString& client_id) {
  QJsonObject json;

  if (!shared::protocol::Deserialize(message, &json)) {
    emit eventOccurred(QString("Ошибка: не удалось разобрать "
                               "JSON сообщение от клиента %1")
                           .arg(client_id));

    return;
  }

  const auto type = json.take(shared::protocol::k_type).toString();

  if (type.isEmpty()) {
    emit eventOccurred(QString("Ошибка: клиент %1 прислал JSON "
                               "без поля type")
                           .arg(client_id));

    return;
  }

  server::types::ClientData data;
  data.client_id = client_id;
  data.type = type;
  data.content = formatContent(type, json);
  data.timestamp = QDateTime::currentDateTime();

  emit clientDataReceived(data);
}

QString TcpServer::formatContent(const QString& type, const QJsonObject& json) {
  if (type == shared::protocol::k_log) {
    return QString("[%1] %2").arg(
        json.value(shared::protocol::k_severity).toString(),
        json.value(shared::protocol::k_message).toString());
  }

  QStringList parts;

  for (auto it = json.begin(); it != json.end(); ++it) {
    parts.append(
        QString("%1=%2").arg(it.key(), it.value().toVariant().toString()));
  }

  return parts.join(", ");
}

}  // namespace server

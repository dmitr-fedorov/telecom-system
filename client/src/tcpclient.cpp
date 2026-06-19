#include "../include/tcpclient.h"

namespace client {

TcpClient::TcpClient(QObject* parent) : QObject(parent) {
  initializeSocket();

  reconnect_timer_.setInterval(reconnect_interval_ms_);
  reconnect_timer_.setSingleShot(true);

  connect(&reconnect_timer_, &QTimer::timeout, this, &TcpClient::tryConnect);
}

void TcpClient::start() { tryConnect(); }

void TcpClient::sendData(const QJsonObject& json) {
  if (!socket_ || socket_->state() != QAbstractSocket::ConnectedState) {
    qWarning() << "Ошибка: попытка отправить данные "
                  "без подключения к серверу";

    return;
  }

  const auto data = shared::protocol::Serialize(json);
  const qint64 written = socket_->write(data);

  if (written == -1) {
    qWarning() << "Ошибка: не удалось отправить данные:"
               << socket_->errorString();

    return;
  }
}

void TcpClient::processMessage(const QByteArray& message) {
  QJsonObject json;

  if (!shared::protocol::Deserialize(message, &json)) {
    qWarning() << "Ошибка: не удалось разобрать "
                  "JSON сообщение от сервера";

    return;
  }

  handleJsonMessage(json);
}

void TcpClient::handleJsonMessage(const QJsonObject& json) {
  const auto type = json[shared::protocol::k_type].toString();

  if (type.isEmpty()) {
    qWarning() << "Ошибка: получено JSON "
                  "сообщение от сервера "
                  "без валидного поля type";

    return;
  }

  if (type == shared::protocol::k_ack) {
    auto client_id = json[shared::protocol::k_client_id].toString();

    if (client_id.isEmpty()) {
      client_id = QStringLiteral("invalid");
    }

    qInfo() << "Получено подтверждение соединения."
            << "Назначенный id:" << client_id;

    return;
  }

  if (type == shared::protocol::k_start_transmission) {
    qInfo() << "Получена команда на начало "
               "отправки данных";

    emit startTransmissionCommandReceived();

    return;
  }

  if (type == shared::protocol::k_stop_transmission) {
    qInfo() << "Получена команда на прекращение "
               "отправки данных";

    emit stopTransmissionCommandReceived();

    return;
  }

  if (type == shared::protocol::k_limits_config) {
    qInfo() << "Получена конфигурация лимитов";

    handleLimitsConfigMessage(json);

    return;
  }

  qWarning() << "Ошибка: получено сообщение "
                "неизвестного типа:"
             << type;
}

void TcpClient::handleLimitsConfigMessage(const QJsonObject& json) {
  shared::types::LimitsConfig config;

  QJsonValue json_value = json.value(shared::protocol::k_latency);

  if (json_value.isDouble()) {
    config.latency = json_value.toDouble();
  }

  json_value = json.value(shared::protocol::k_errors);

  if (json_value.isDouble()) {
    config.errors = json_value.toInt();
  }

  json_value = json.value(shared::protocol::k_cpu_usage);

  if (json_value.isDouble()) {
    config.cpu_usage = json_value.toInt();
  }

  json_value = json.value(shared::protocol::k_temperature);

  if (json_value.isDouble()) {
    config.temperature = json_value.toDouble();
  }

  emit limitsConfigReceived(config);
}

void TcpClient::resetSocketAndScheduleReconnect() {
  read_buffer_.clear();

  if (socket_) {
    socket_->disconnect(this);
    socket_->deleteLater();
    socket_ = nullptr;
  }

  initializeSocket();

  scheduleReconnect();
}

void TcpClient::initializeSocket() {
  socket_ = new QTcpSocket(this);

  connect(socket_, &QTcpSocket::connected, this, &TcpClient::onConnected);

  connect(socket_, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);

  connect(socket_, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);

  connect(socket_, &QTcpSocket::errorOccurred, this,
          &TcpClient::onErrorOccurred);
}

void TcpClient::scheduleReconnect() {
  if (reconnect_timer_.isActive()) {
    return;
  }

  qInfo() << "Повторное соединение "
             "запланировано через"
          << reconnect_interval_ms_ << "мс";

  reconnect_timer_.start();
}

void TcpClient::onConnected() {
  qInfo().nospace() << "Подключено к серверу: " << server_address_ << ':'
                    << server_port_;
}

void TcpClient::onDisconnected() {
  qInfo().nospace() << "Отключено от сервера: " << server_address_ << ':'
                    << server_port_;

  emit disconnected();

  resetSocketAndScheduleReconnect();
}

void TcpClient::onReadyRead() {
  read_buffer_.append(socket_->readAll());

  while (true) {
    const int delimiter_index =
        read_buffer_.indexOf(shared::protocol::tcp_packet_delimeter);

    if (delimiter_index == -1) {
      break;
    }

    const auto message = read_buffer_.left(delimiter_index);

    read_buffer_.remove(0, delimiter_index + 1);

    processMessage(message);
  }
}

void TcpClient::tryConnect() {
  if (!socket_ || socket_->state() != QAbstractSocket::UnconnectedState) {
    return;
  }

  qInfo().nospace() << "Подключение к серверу: " << server_address_ << ':'
                    << server_port_ << "...";

  socket_->connectToHost(server_address_, server_port_);
}

void TcpClient::onErrorOccurred(QAbstractSocket::SocketError error) {
  Q_UNUSED(error);

  qWarning() << "Ошибка сокета:" << socket_->errorString();

  if (socket_->state() != QAbstractSocket::ConnectedState) {
    resetSocketAndScheduleReconnect();
  }
}

}  // namespace client

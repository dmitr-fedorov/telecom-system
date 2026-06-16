#pragma once

#include <QDebug>
#include <QHostAddress>
#include <QJsonObject>
#include <QObject>
#include <QTcpSocket>
#include <QTimer>

#include "../../shared/include/protocol.h"
#include "../../shared/include/types.h"

namespace client {

// Реализует сетевую часть работы клиента.
// Принимает команды от сервера,
// испускает сигналы в соответствии с полученными командами.
// Принимает сгенерированные данные в слот и отправляет их серверу.
// Обрабатывает сетевые ошибки.
// Выводит сообщения о событиях в консоль.
class TcpClient : public QObject {
  Q_OBJECT

 public:
  explicit TcpClient(QObject* parent = nullptr);

  void start();

 public slots:
  void sendData(const QJsonObject& json);

 signals:
  void startTransmissionCommandReceived();

  void stopTransmissionCommandReceived();

  void limitsConfigReceived(shared::types::LimitsConfig& config);

 private:
  static constexpr quint16 server_port_ = 12345;

  static constexpr auto server_address_ = "127.0.0.1";

  static constexpr int reconnect_interval_ms_ = 5000;

  QTcpSocket socket_;

  QByteArray read_buffer_;

  QTimer reconnect_timer_;

  void processMessage(const QByteArray& message);

  void handleJsonMessage(const QJsonObject& json);

  void handleLimitsConfigMessage(const QJsonObject& json);

  void scheduleReconnect();

 private slots:
  void onConnected();

  void onDisconnected();

  void onReadyRead();

  void tryConnect();

  void onErrorOccurred(QAbstractSocket::SocketError error);
};

}  // namespace client

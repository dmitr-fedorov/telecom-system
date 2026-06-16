#pragma once

#include <QDateTime>
#include <QHash>
#include <QHostAddress>
#include <QJsonObject>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "../../shared/include/protocol.h"
#include "../../shared/include/types.h"
#include "../include/types.h"

namespace server {

class TcpServer : public QObject {
  Q_OBJECT

 public:
  explicit TcpServer(QObject* parent = nullptr);

 public slots:
  void startServer();
  void stopServer();

  void startClientsDataTransmission();
  void stopClientsDataTransmission();

  void applyLimitsConfig(const shared::types::LimitsConfig& config);

 signals:
  void serverStarted();

  void clientConnectionStateChanged(const server::types::ClientInfo& info);

  void clientsTransmittingStateChanged(bool transmitting);

  void clientDataReceived(const server::types::ClientData& data);

  void eventOccurred(const QString& event);

 private:
  struct ClientContext {
    QString client_id;

    QByteArray buffer;
  };

  static constexpr quint16 server_port_ = 12345;

  QTcpServer* server_ = nullptr;

  QHash<QTcpSocket*, ClientContext> clients_;

  bool is_clients_running_ = false;

  uint64_t id_counter_ = 0;

  shared::types::LimitsConfig last_limits_config_;

  QString getNewClientId();

  bool sendMessage(QTcpSocket* socket, const QJsonObject& json);

  bool broadcastMessage(const QJsonObject& json);

  void sendAck(QTcpSocket* socket, const QString& client_id);

  void sendStartCommand(QTcpSocket* socket, const QString& client_id);

  void sendLastLimitsConfig(QTcpSocket* socket, const QString& client_id);

  QJsonObject toJson(const shared::types::LimitsConfig& config);

  void processMessage(const QByteArray& message, const QString& client_id);

  QString formatContent(const QString& type, const QJsonObject& json);

 private slots:
  void onNewConnection();

  void onClientDisconnected();

  void onReadyRead();

  void onSocketError(QAbstractSocket::SocketError error);
};

}  // namespace server

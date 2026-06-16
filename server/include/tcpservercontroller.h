#pragma once

#include <QDateTime>
#include <QObject>
#include <QThread>

#include "../../shared/include/types.h"
#include "../include/tcpserver.h"
#include "../include/types.h"

namespace server {

// Содержит TcpServer, запускает его в отдельном потоке и корректно его удаляет.
// Является посредником между TcpServer и GUI.
class TcpServerController : public QObject {
  Q_OBJECT

 public:
  explicit TcpServerController(QObject* parent = nullptr);

  ~TcpServerController();

 public slots:
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
  QThread server_thread_;

  TcpServer* tcp_server_ = nullptr;
};

}  // namespace server

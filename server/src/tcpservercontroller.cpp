#include "../include/tcpservercontroller.h"

namespace server {

TcpServerController::TcpServerController(QObject* parent) : QObject(parent) {
  tcp_server_ = new TcpServer();
  tcp_server_->moveToThread(&server_thread_);

  connect(&server_thread_, &QThread::started, tcp_server_,
          &TcpServer::startServer);

  connect(&server_thread_, &QThread::finished, tcp_server_,
          &QObject::deleteLater);

  connect(tcp_server_, &TcpServer::clientConnectionStateChanged, this,
          &TcpServerController::clientConnectionStateChanged);

  connect(tcp_server_, &TcpServer::eventOccurred, this,
          &TcpServerController::eventOccurred);

  connect(tcp_server_, &TcpServer::clientsTransmittingStateChanged, this,
          &TcpServerController::clientsTransmittingStateChanged);

  connect(tcp_server_, &TcpServer::clientDataReceived, this,
          &TcpServerController::clientDataReceived);

  connect(tcp_server_, &TcpServer::serverStarted, this,
          &TcpServerController::serverStarted);

  server_thread_.start();
}

TcpServerController::~TcpServerController() {
  if (tcp_server_ != nullptr) {
    QMetaObject::invokeMethod(tcp_server_, &TcpServer::stopServer,
                              Qt::BlockingQueuedConnection);
  }

  server_thread_.quit();
  server_thread_.wait();
}

void TcpServerController::startClientsDataTransmission() {
  QMetaObject::invokeMethod(tcp_server_,
                            &TcpServer::startClientsDataTransmission,
                            Qt::QueuedConnection);
}

void TcpServerController::stopClientsDataTransmission() {
  QMetaObject::invokeMethod(tcp_server_,
                            &TcpServer::stopClientsDataTransmission,
                            Qt::QueuedConnection);
}

void TcpServerController::applyLimitsConfig(
    const shared::types::LimitsConfig& config) {
  QMetaObject::invokeMethod(
      tcp_server_, [=]() { tcp_server_->applyLimitsConfig(config); },
      Qt::QueuedConnection);
}

}  // namespace server

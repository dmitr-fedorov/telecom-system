#pragma once

#include <QDateTime>
#include <QObject>
#include <QThread>

#include "../include/tcpserver.h"
#include "../../shared/include/sharedtypes.h"

class TcpServerController : public QObject
{
    Q_OBJECT

public:
    explicit TcpServerController(
        QObject* parent = nullptr);

    ~TcpServerController();

public slots:
    void startClients();

    void stopClients();

    void applyLimitsConfig(
        const sharedTypes::LimitsConfig& config);

signals:
    void serverStarted();

    void clientConnectionStateChanged(
        const QString& client_id,
        const QString& ip_address,
        const QString& state);

    void clientsRunningStateChanged(
        bool running);

    void eventOccurred(
        const QString& event);

    void clientDataReceived(
        const QString& clientId,
        const QString& type,
        const QString& content,
        const QDateTime& timestamp);

private:
    QThread _server_thread;

    TcpServer* _tcp_server = nullptr;
};

#pragma once

#include <QDateTime>
#include <QObject>
#include <QThread>

#include "../../shared/include/sharedtypes.h"

#include "../include/apptypes.h"
#include "../include/tcpserver.h"

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
        const appTypes::ClientInfo& info);

    void clientsRunningStateChanged(
        bool running);

    void eventOccurred(
        const QString& event);

    void clientDataReceived(
        const appTypes::ClientData& data);

private:
    QThread _server_thread;

    TcpServer* _tcp_server = nullptr;
};

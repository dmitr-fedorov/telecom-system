#pragma once

#include <QDateTime>
#include <QObject>
#include <QThread>

#include "../include/tcpservermanager.h"

class ServerController : public QObject
{
    Q_OBJECT

public:
    explicit ServerController(
        QObject* parent = nullptr);

    ~ServerController();

public slots:
    void startClients();

    void stopClients();

    void applyConfiguration(
        int limit_value);

signals:
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

    TcpServerManager* _server = nullptr;
};

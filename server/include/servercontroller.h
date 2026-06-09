#pragma once

#include <QDateTime>
#include <QObject>
#include <QThread>

#include "../include/tcpservermanager.h"

struct ClientData
{
    QString client_id;
    QString type;
    QString content;
    QString timestamp;
};

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
    void clientsRunningStateChanged(
        bool running);

    void clientInfoReceived(
        const ClientInfo& info);

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

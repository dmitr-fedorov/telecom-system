#pragma once

#include <QHash>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QRandomGenerator>
#include <QObject>

#include "../../shared/include/protocol.h"

struct ClientInfo
{
    QString client_id;
    QString ip_address;
    quint16 port = 0;
    QString status;
};

class TcpServerManager : public QObject
{
    Q_OBJECT

public:
    explicit TcpServerManager(
        QObject* parent = nullptr);

public slots:
    void startServer();

    void stopServer();

    void startClients();

    void stopClients();

    void applyConfiguration(
        int limit_value);

signals:
    void clientConnected(
        const ClientInfo& info);

    void clientDisconnected(
        const QString& client_id);

    void eventOccurred(
        const QString& event);

private:
    struct ClientContext
    {
        QString client_id;

        QByteArray buffer;
    };

    static constexpr quint16 _SERVER_PORT = 12345;

    QTcpServer* _server = nullptr;

    QHash<QTcpSocket*, ClientContext> _clients;

    bool _is_clients_running = false;

    std::atomic_uint64_t _id_counter = 0;

    QString getNewClientId();

    void sendMessage(
        QTcpSocket* socket,
        const QJsonObject& object);

    void broadcastMessage(
        const QJsonObject& object);

    void sendAck(
        QTcpSocket* socket,
        const QString& client_id);

private slots:
    void onNewConnection();

    void onClientDisconnected();

    void onReadyRead();
};

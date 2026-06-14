#pragma once

#include <QDateTime>
#include <QHash>
#include <QHostAddress>
#include <QJsonObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>

#include "../include/apptypes.h"

#include "../../shared/include/protocol.h"
#include "../../shared/include/sharedtypes.h"

class TcpServer : public QObject
{
    Q_OBJECT

public:
    explicit TcpServer(
        QObject* parent = nullptr);

public slots:
    void startServer();

    void stopServer();

    void startClients();

    void stopClients();

    void applyLimitsConfig(
        const sharedTypes::LimitsConfig& config);

signals:
    void serverStarted();

    void clientsRunningStateChanged(
        bool running);

    void clientConnectionStateChanged(
        const appTypes::ClientInfo& info);

    void eventOccurred(
        const QString& event);

    void clientDataReceived(
        const appTypes::ClientData& data);

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

    sharedTypes::LimitsConfig _last_limits_config;

    QString getNewClientId();

    bool sendMessage(
        QTcpSocket* socket,
        const QJsonObject& object);

    bool broadcastMessage(
        const QJsonObject& object);

    void sendAck(
        QTcpSocket* socket,
        const QString& client_id);

    void sendStartCommand(
        QTcpSocket* socket,
        const QString& client_id);

    void sendLastLimitsConfig(
        QTcpSocket* socket,
        const QString& client_id);

    QJsonObject toJson(
        const sharedTypes::LimitsConfig& config);

    void processMessage(
        const QByteArray& message,
        const QString& client_id);

    QString formatContent(
        const QString& type,
        const QJsonObject& object);

private slots:
    void onNewConnection();

    void onClientDisconnected();

    void onReadyRead();

    void onSocketError(
        QAbstractSocket::SocketError);
};

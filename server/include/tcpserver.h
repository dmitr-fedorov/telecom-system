#pragma once

#include <QDateTime>
#include <QHash>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>

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
        const QString& client_id,
        const QString& ip_address,
        const QString& state);

    void eventOccurred(
        const QString& event);

    void clientDataReceived(
        const QString& clientId,
        const QString& type,
        const QString& content,
        const QDateTime& timestamp);

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
        const QString& clientId);

    void sendStartCommand(
        QTcpSocket* socket,
        const QString& clientId);

    void sendLastLimitsConfig(
        QTcpSocket* socket,
        const QString& clientId);

    QJsonObject toJson(
        const sharedTypes::LimitsConfig& config);

    void processMessage(
        const QString& clientId,
        const QByteArray& message);

    void processLogMessage(
        const QString& clientId,
        const QJsonObject& object);

private slots:
    void onNewConnection();

    void onClientDisconnected();

    void onReadyRead();

    void onSocketError(
        QAbstractSocket::SocketError);
};

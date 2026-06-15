#pragma once

#include <QHostAddress>
#include <QJsonObject>
#include <QDebug>
#include <QObject>
#include <QTcpSocket>
#include <QTimer>

#include "../../shared/include/protocol.h"
#include "../../shared/include/sharedtypes.h"

class TcpClient : public QObject
{
    Q_OBJECT

public:
    explicit TcpClient(
        QObject* parent = nullptr);

    void start();

    void sendData(
        const QJsonObject& json);

signals:
    void startCommandReceived();

    void stopCommandReceived();

    void limitsConfigReceived(
        sharedTypes::LimitsConfig& config);

private slots:
    void onConnected();

    void onDisconnected();

    void onReadyRead();

    void onErrorOccurred(
        QAbstractSocket::SocketError);

    void tryConnect();

private:
    QTcpSocket _socket;

    QByteArray _read_buffer;

    QTimer _reconnect_timer;

    void processMessage(
        const QByteArray& message);

    void handleJsonMessage(
        const QJsonObject& json);

    void handleLimitsConfigMessage(
        const QJsonObject& json);

    void scheduleReconnect();
};

#pragma once

#include <QHostAddress>
#include <QJsonObject>
#include <QDebug>
#include <QObject>
#include <QTcpSocket>
#include <QTimer>

#include "../include/protocol.h"

class ClientApp : public QObject
{
    Q_OBJECT

public:
    explicit ClientApp(QObject* parent = nullptr);

    void start();

private slots:
    void onConnected();

    void onDisconnected();

    void onReadyRead();

    void onErrorOccurred(QAbstractSocket::SocketError error);

    void tryConnect();

private:
    void processMessage(const QByteArray& message);

    void handleJsonMessage(const QJsonObject& object);

    void scheduleReconnect();

private:
    QTcpSocket _socket;

    QByteArray _read_buffer;

    QTimer _reconnect_timer;

    bool _is_connection_accepted = false;
};

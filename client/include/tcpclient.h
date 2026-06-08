#pragma once

#include <QHostAddress>
#include <QJsonObject>
#include <QDebug>
#include <QObject>
#include <QTcpSocket>
#include <QTimer>

#include "../../shared/include/protocol.h"

class TcpClient : public QObject
{
    Q_OBJECT

public:
    explicit TcpClient(QObject* parent = nullptr);

    void start();

    void sendData(const QJsonObject& object);

signals:
    void startCommandReceived();

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
};

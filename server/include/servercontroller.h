#pragma once

#include <QDateTime>
#include <QObject>

struct ClientInfo
{
    QString client_id;
    QString ip_address;
    QString status;
};

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

signals:
    void ClientInfoReceived(
        const ClientInfo& info);

    void ClientDataReceived(
        const ClientData& data);

    void EventOccurred(
        const QString& text);

public slots:
    void StartStopClients();

    void ApplyConfiguration(
        int limit_value);

private:
    bool _is_clients_started = false;
};

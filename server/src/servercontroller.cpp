#include "../include/servercontroller.h"

ServerController::ServerController(
    QObject* parent)
    : QObject(parent)
{
}

void ServerController::StartStopClients()
{
    if (!_is_clients_started) {
        // TcpServerManager->sendJsonCommand
        emit EventOccurred(
            "Start command sent to clients");
    }
    else
    {
        // TcpServerManager->sendJsonCommand
        emit EventOccurred(
            "Stop command sent to clients");
    }



}

void ServerController::ApplyConfiguration(
    int limit_value)
{
    // TcpServerManager->sendJsonConfig to clients
    emit EventOccurred(
        QString("Configuration applied. "
                "Limit value = %1")
            .arg(limit_value));
}

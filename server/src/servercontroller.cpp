#include "../include/servercontroller.h"

ServerController::ServerController(
    QObject* parent)
    : QObject(parent)
{
    _server = new TcpServerManager();
    _server->moveToThread(&_server_thread);

    connect(&_server_thread, &QThread::started,
            _server, &TcpServerManager::startServer);

    connect(&_server_thread, &QThread::finished,
            _server, &QObject::deleteLater);

    connect(_server, &TcpServerManager::eventOccurred,
            this, &ServerController::eventOccurred);

    _server_thread.start();
}

ServerController::~ServerController()
{
    if (_server != nullptr)
    {
        QMetaObject::invokeMethod(
            _server,
            &TcpServerManager::stopServer,
            Qt::BlockingQueuedConnection);
    }

    _server_thread.quit();
    _server_thread.wait();
}

void ServerController::startClients()
{
    QMetaObject::invokeMethod(
        _server,
        &TcpServerManager::startClients,
        Qt::QueuedConnection);
}

void ServerController::stopClients()
{
    QMetaObject::invokeMethod(
        _server,
        &TcpServerManager::stopClients,
        Qt::QueuedConnection);
}

void ServerController::applyConfiguration(
    int limit_value)
{
    // TcpServerManager->sendJsonConfig to clients

    emit eventOccurred(
        QString("Configuration applied. "
                "Limit value = %1")
            .arg(limit_value));
}

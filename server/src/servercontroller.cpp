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

    connect(_server, &TcpServerManager::clientsRunningStateChanged,
            this, &ServerController::clientsRunningStateChanged);

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
    QMetaObject::invokeMethod(
        _server,
        [=]()
        {
            _server->applyConfiguration(limit_value);
        },
        Qt::QueuedConnection);
}

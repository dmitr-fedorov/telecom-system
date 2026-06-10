#include "../include/tcpservercontroller.h"

TcpServerController::TcpServerController(
    QObject* parent)
    : QObject(parent)
{
    _server = new TcpServer();
    _server->moveToThread(&_server_thread);

    connect(&_server_thread, &QThread::started,
            _server, &TcpServer::startServer);

    connect(&_server_thread, &QThread::finished,
            _server, &QObject::deleteLater);

    connect(_server, &TcpServer::clientConnectionStateChanged,
            this, &TcpServerController::clientConnectionStateChanged);

    connect(_server, &TcpServer::eventOccurred,
            this, &TcpServerController::eventOccurred);

    connect(_server, &TcpServer::clientsRunningStateChanged,
            this, &TcpServerController::clientsRunningStateChanged);

    connect(_server, &TcpServer::clientDataReceived,
            this, &TcpServerController::clientDataReceived);

    _server_thread.start();
}

TcpServerController::~TcpServerController()
{
    if (_server != nullptr)
    {
        QMetaObject::invokeMethod(
            _server,
            &TcpServer::stopServer,
            Qt::BlockingQueuedConnection);
    }

    _server_thread.quit();
    _server_thread.wait();
}

void TcpServerController::startClients()
{
    QMetaObject::invokeMethod(
        _server,
        &TcpServer::startClients,
        Qt::QueuedConnection);
}

void TcpServerController::stopClients()
{
    QMetaObject::invokeMethod(
        _server,
        &TcpServer::stopClients,
        Qt::QueuedConnection);
}

void TcpServerController::applyConfiguration(
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

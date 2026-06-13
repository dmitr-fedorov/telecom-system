#include "../include/tcpservercontroller.h"

TcpServerController::TcpServerController(
    QObject* parent)
    : QObject(parent)
{
    _tcp_server = new TcpServer();
    _tcp_server->moveToThread(&_server_thread);

    connect(&_server_thread, &QThread::started,
            _tcp_server, &TcpServer::startServer);

    connect(&_server_thread, &QThread::finished,
            _tcp_server, &QObject::deleteLater);

    connect(_tcp_server, &TcpServer::clientConnectionStateChanged,
            this, &TcpServerController::clientConnectionStateChanged);

    connect(_tcp_server, &TcpServer::eventOccurred,
            this, &TcpServerController::eventOccurred);

    connect(_tcp_server, &TcpServer::clientsRunningStateChanged,
            this, &TcpServerController::clientsRunningStateChanged);

    connect(_tcp_server, &TcpServer::clientDataReceived,
            this, &TcpServerController::clientDataReceived);

    connect(_tcp_server, &TcpServer::serverStarted,
            this, &TcpServerController::serverStarted);

    _server_thread.start();
}

TcpServerController::~TcpServerController()
{
    if (_tcp_server != nullptr)
    {
        QMetaObject::invokeMethod(
            _tcp_server,
            &TcpServer::stopServer,
            Qt::BlockingQueuedConnection);
    }

    _server_thread.quit();
    _server_thread.wait();
}

void TcpServerController::startClients()
{
    QMetaObject::invokeMethod(
        _tcp_server,
        &TcpServer::startClients,
        Qt::QueuedConnection);
}

void TcpServerController::stopClients()
{
    QMetaObject::invokeMethod(
        _tcp_server,
        &TcpServer::stopClients,
        Qt::QueuedConnection);
}

void TcpServerController::applyLimitsConfig(
    const sharedTypes::LimitsConfig& config)
{
    QMetaObject::invokeMethod(
        _tcp_server,
        [=]()
        {
            _tcp_server->applyLimitsConfig(config);
        },
        Qt::QueuedConnection);
}

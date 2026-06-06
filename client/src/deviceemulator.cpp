#include "../include/deviceemulator.h"

DeviceEmulator::DeviceEmulator(QObject* parent)
    : QObject(parent)
{
    _tcpClient = new TcpClient(this);
    _genScheduler
        = new DataGenerationScheduler(this);

    connect(_tcpClient, &TcpClient::startCommandReceived,
            _genScheduler, &DataGenerationScheduler::start);

    connect(_genScheduler, &DataGenerationScheduler::dataGenerated,
            _tcpClient, &TcpClient::sendGeneratedData);
}

void DeviceEmulator::start()
{
    _tcpClient->start();
}

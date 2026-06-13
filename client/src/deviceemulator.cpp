#include "../include/deviceemulator.h"

DeviceEmulator::DeviceEmulator(QObject* parent)
    : QObject(parent)
{
    _tcpClient = new TcpClient(this);

    _genScheduler =
        new DataGenerationScheduler(this);

    _dataValidator =
        new DataValidator(this);

    connect(_tcpClient,
            &TcpClient::startCommandReceived,
            _genScheduler,
            &DataGenerationScheduler::start);

    connect(_tcpClient,
            &TcpClient::stopCommandReceived,
            _genScheduler,
            &DataGenerationScheduler::stop);

    connect(_genScheduler,
            &DataGenerationScheduler::dataGenerated,
            _tcpClient,
            &TcpClient::sendData);

    connect(_tcpClient,
            &TcpClient::limitsConfigReceived,
            _dataValidator,
            &DataValidator::applyLimitsConfig);

    connect(_genScheduler,
            &DataGenerationScheduler::dataGenerated,
            _dataValidator,
            &DataValidator::validate);

    connect(_dataValidator,
            &DataValidator::warningLogReady,
            _tcpClient,
            &TcpClient::sendData);
}

void DeviceEmulator::start()
{
    _tcpClient->start();
}

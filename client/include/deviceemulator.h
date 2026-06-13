#pragma once

#include <QObject>

#include "tcpclient.h"
#include "datagenerationscheduler.h"
#include "datavalidator.h"

class DeviceEmulator : public QObject
{
    Q_OBJECT

public:
    explicit DeviceEmulator(QObject* parent = nullptr);

    void start();

private:
    TcpClient* _tcpClient = nullptr;

    DataGenerationScheduler*
        _genScheduler = nullptr;

    DataValidator*
        _dataValidator = nullptr;
};

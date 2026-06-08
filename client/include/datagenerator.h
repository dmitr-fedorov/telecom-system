#pragma once

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QRandomGenerator>

#include "../../shared/include/protocol.h"

class DataGenerator
{
public:
    static QJsonObject GenerateRandomData();

private:
    enum class MessageSize
    {
        Short,
        Medium,
        Long
    };

    static QJsonObject GenerateNetworkMetrics();

    static QJsonObject GenerateDeviceStatus();

    static QJsonObject GenerateLog();

    static MessageSize GenerateMessageSize();

    static QString GenerateLogText(
        MessageSize size);

    static double RandomDouble(double min, double max);
};

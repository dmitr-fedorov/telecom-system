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
    static QJsonObject CreateNetworkMetrics();

    static QJsonObject CreateDeviceStatus();

    static QJsonObject CreateLogMessage();

private:
    enum class MessageSize
    {
        Short,
        Medium,
        Long
    };

private:
    static QString GenerateTimestamp();

    static MessageSize GenerateMessageSize();

    static QString GenerateSeverity();

    static QString GenerateLogText(
        MessageSize size);

    static QJsonArray GenerateConnections();

    static QJsonArray GenerateModuleStatuses();

    static double RandomDouble(double min, double max);
};

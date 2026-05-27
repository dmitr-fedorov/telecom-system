#pragma once

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QRandomGenerator>

#include "../../shared/include/protocol.h"

class MessageFactory
{
public:
    static QJsonObject CreateNetworkMetrics(
        const QString& client_id);

    static QJsonObject CreateDeviceStatus(
        const QString& client_id);

    static QJsonObject CreateLogMessage(
        const QString& client_id);

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

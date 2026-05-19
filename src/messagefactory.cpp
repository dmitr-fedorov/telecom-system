#include "../include/messagefactory.h"

QJsonObject MessageFactory::CreateNetworkMetrics(
    const QString& client_id)
{
    const MessageSize size =
        GenerateMessageSize();

    QJsonObject object;

    object[protocol::kType] =
        protocol::kNetworkMetrics;

    object[protocol::kClientId] =
        client_id;

    object[protocol::kTimestamp] =
        GenerateTimestamp();

    object["bandwidth"] =
        RandomDouble(10.0, 1000.0);

    object["latency"] =
        RandomDouble(1.0, 150.0);

    if (size != MessageSize::Short)
    {
        object["packet_loss"] =
            RandomDouble(0.0, 5.0);

        object["jitter"] =
            RandomDouble(0.0, 20.0);

        object["signal_strength"] =
            RandomDouble(40.0, 100.0);
    }

    if (size == MessageSize::Long)
    {
        object["connections"] =
            GenerateConnections();

        QJsonObject statistics;

        statistics["sent_packets"] =
            QRandomGenerator::global()->bounded(
                10000,
                500000);

        statistics["received_packets"] =
            QRandomGenerator::global()->bounded(
                10000,
                500000);

        statistics["errors"] =
            QRandomGenerator::global()->bounded(
                0,
                1000);

        object["statistics"] =
            statistics;
    }

    return object;
}

QJsonObject MessageFactory::CreateDeviceStatus(
    const QString& client_id)
{
    const MessageSize size =
        GenerateMessageSize();

    QJsonObject object;

    object[protocol::kType] =
        protocol::kDeviceStatus;

    object[protocol::kClientId] =
        client_id;

    object[protocol::kTimestamp] =
        GenerateTimestamp();

    object["uptime"] =
        QRandomGenerator::global()->bounded(
            1000,
            1000000);

    object["cpu_usage"] =
        QRandomGenerator::global()->bounded(
            0,
            100);

    if (size != MessageSize::Short)
    {
        object["memory_usage"] =
            QRandomGenerator::global()->bounded(
                0,
                100);

        object["temperature"] =
            RandomDouble(25.0, 85.0);

        object["active_tasks"] =
            QRandomGenerator::global()->bounded(
                0,
                500);
    }

    if (size == MessageSize::Long)
    {
        object["modules"] =
            GenerateModuleStatuses();

        QJsonObject system_state;

        system_state["storage_available"] =
            QRandomGenerator::global()->bounded(
                10,
                500);

        system_state["active_processes"] =
            QRandomGenerator::global()->bounded(
                20,
                300);

        system_state["warnings"] =
            QRandomGenerator::global()->bounded(
                0,
                10);

        object["system_state"] =
            system_state;
    }

    return object;
}

QJsonObject MessageFactory::CreateLogMessage(
    const QString& client_id)
{
    MessageSize size;

    const int random_value =
        QRandomGenerator::global()->bounded(100);

    if (random_value < 20)
    {
        size = MessageSize::Short;
    }
    else if (random_value < 50)
    {
        size = MessageSize::Medium;
    }
    else
    {
        size = MessageSize::Long;
    }

    QJsonObject object;

    object[protocol::kType] =
        protocol::kLog;

    object[protocol::kClientId] =
        client_id;

    object[protocol::kTimestamp] =
        GenerateTimestamp();

    object["severity"] =
        GenerateSeverity();

    object["message"] =
        GenerateLogText(size);

    return object;
}

QString MessageFactory::GenerateTimestamp()
{
    return QDateTime::currentDateTimeUtc()
    .toString(Qt::ISODate);
}

MessageFactory::MessageSize
MessageFactory::GenerateMessageSize()
{
    const int value =
        QRandomGenerator::global()->bounded(3);

    if (value == 0)
    {
        return MessageSize::Short;
    }

    if (value == 1)
    {
        return MessageSize::Medium;
    }

    return MessageSize::Long;
}

QString MessageFactory::GenerateSeverity()
{
    constexpr const char* severities[] =
        {
            "INFO",
            "WARNING",
            "ERROR"
        };

    return severities[
        QRandomGenerator::global()->bounded(3)];
}

QString MessageFactory::GenerateLogText(
    MessageSize size)
{
    if (size == MessageSize::Short)
    {
        return
            "Connection restarted";
    }

    if (size == MessageSize::Medium)
    {
        return
            "Temporary communication issue detected "
            "during data synchronization. "
            "Recovery completed successfully.";
    }

    return
        "Application detected temporary instability "
        "during data synchronization procedure. "
        "Several reconnect attempts were required "
        "to restore communication with remote "
        "services. Monitoring subsystem reported "
        "increased response time and intermittent "
        "packet processing delays during the "
        "recovery interval.";
}

QJsonArray MessageFactory::GenerateConnections()
{
    QJsonArray connections;

    const int count =
        QRandomGenerator::global()->bounded(2, 6);

    for (int i = 0; i < count; ++i)
    {
        QJsonObject connection;

        connection["address"] =
            QString("192.168.1.%1")
                .arg(i + 1);

        connection["latency"] =
            RandomDouble(1.0, 100.0);

        connection["status"] =
            QRandomGenerator::global()->bounded(2)
                ? "active"
                : "inactive";

        connections.append(connection);
    }

    return connections;
}

QJsonArray MessageFactory::GenerateModuleStatuses()
{
    constexpr const char* module_names[] =
        {
            "Network",
            "Storage",
            "Security",
            "Monitoring",
            "Database"
        };

    constexpr const char* statuses[] =
        {
            "OK",
            "WARNING",
            "ERROR"
        };

    QJsonArray modules;

    for (const auto* module_name : module_names)
    {
        QJsonObject module;

        module["name"] =
            module_name;

        module["status"] =
            statuses[
                QRandomGenerator::global()->bounded(3)];

        modules.append(module);
    }

    return modules;
}

double MessageFactory::RandomDouble(
    double min,
    double max)
{
    return min +
           (max - min) *
               QRandomGenerator::global()->generateDouble();
}

#include "../include/datagenerator.h"

QJsonObject DataGenerator::GenerateRandomData()
{
    const auto message_type =
        QRandomGenerator::global()->bounded(3);

    QJsonObject result;

    switch (message_type)
    {
        case 0:
        {
            result = GenerateNetworkMetrics();

            break;
        }

        case 1:
        {
            result = GenerateDeviceStatus();

            break;
        }

        default:
        {
            result = GenerateLog();

            break;
        }
    }

    return result;
}

QJsonObject DataGenerator::GenerateNetworkMetrics()
{
    const MessageSize size =
        GenerateMessageSize();

    QJsonObject result;

    result[protocol::kType] =
        protocol::kNetworkMetrics;

    result["bandwidth"] =
        RandomDouble(10.0, 1000.0);

    if (size >= MessageSize::Medium)
    {
        result["latency"] =
            RandomDouble(1.0, 150.0);

        result["packet_loss"] =
            RandomDouble(0.0, 5.0);

        result["signal_strength"] =
            RandomDouble(40.0, 100.0);
    }

    if (size == MessageSize::Long)
    {
        result["mtu"] =
            RandomDouble(576.0, 9000.0);

        result["rtt"] =
            RandomDouble(10.0, 900000.0);

        result["link_speed"] =
            RandomDouble(100.0, 900000.0);

        result["jitter"] =
            QRandomGenerator::global()->bounded(
                1,
                30);

        result["throughput"] =
            QRandomGenerator::global()->bounded(
                10,
                1500);

        result["sent_packets"] =
            QRandomGenerator::global()->bounded(
                100000,
                500000);

        result["received_packets"] =
            QRandomGenerator::global()->bounded(
                100000,
                500000);

        result["errors"] =
            QRandomGenerator::global()->bounded(
                0,
                1000);
    }

    return result;
}

QJsonObject DataGenerator::GenerateDeviceStatus()
{
    const MessageSize size =
        GenerateMessageSize();

    QJsonObject result;

    result[protocol::kType] =
        protocol::kDeviceStatus;

    result["uptime"] =
        QRandomGenerator::global()->bounded(
            1000,
            1000000);

    if (size >= MessageSize::Medium)
    {
        result["cpu_usage"] =
            QRandomGenerator::global()->bounded(
                0,
                100);

        result["memory_usage"] =
            QRandomGenerator::global()->bounded(
                0,
                100);

        result["temperature"] =
            RandomDouble(25.0, 100.0);
    }

    if (size == MessageSize::Long)
    {
        result["system_up_time"] =
            RandomDouble(1.0, 10000.0);

        result["power_supply"] =
            QRandomGenerator::global()->bounded(
                0,
                10);

        result["buffer_misses"] =
            QRandomGenerator::global()->bounded(
                0,
                100000);

        result["active_tasks"] =
            QRandomGenerator::global()->bounded(
                0,
                500);

        result["storage_available"] =
            QRandomGenerator::global()->bounded(
                10,
                500);

        result["active_processes"] =
            QRandomGenerator::global()->bounded(
                20,
                300);

        result["warnings"] =
            QRandomGenerator::global()->bounded(
                0,
                10);
    }

    return result;
}

QJsonObject DataGenerator::GenerateLog()
{
    const MessageSize size =
        GenerateMessageSize();

    QJsonObject result;

    result[protocol::kType] =
        protocol::kLog;

    result["severity"] = "INFO";

    result["message"] =
        GenerateLogText(size);

    return result;
}

DataGenerator::MessageSize
DataGenerator::GenerateMessageSize()
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

QString DataGenerator::GenerateLogText(
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

double DataGenerator::RandomDouble(double min, double max)
{
    const int minInt
        = static_cast<int>(min * 100.0);

    const int maxInt
        = static_cast<int>(max * 100.0);

    return QRandomGenerator::global()->bounded(minInt, maxInt) / 100.0;
}

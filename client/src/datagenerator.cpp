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

    result[protocol::kBandwidth] =
        RandomDouble(10.0, 1000.0);

    if (size >= MessageSize::Medium)
    {
        result[protocol::kLatency] =
            RandomDouble(1.0, 150.0);

        result[protocol::kPacketLoss] =
            RandomDouble(0.0, 5.0);

        result[protocol::kSignalStrength] =
            RandomDouble(40.0, 100.0);
    }

    if (size == MessageSize::Long)
    {
        result[protocol::kMtu] =
            RandomDouble(576.0, 9000.0);

        result[protocol::kRtt] =
            RandomDouble(10.0, 900000.0);

        result[protocol::kLinkSpeed] =
            RandomDouble(100.0, 900000.0);

        result[protocol::kJitter] =
            QRandomGenerator::global()->bounded(
                1,
                30);

        result[protocol::kThroughput] =
            QRandomGenerator::global()->bounded(
                10,
                1500);

        result[protocol::kSentPackets] =
            QRandomGenerator::global()->bounded(
                100000,
                500000);

        result[protocol::kReceivedPackets] =
            QRandomGenerator::global()->bounded(
                100000,
                500000);

        result[protocol::kErrors] =
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

    result[protocol::kUptime] =
        QRandomGenerator::global()->bounded(
            1000,
            1000000);

    if (size >= MessageSize::Medium)
    {
        result[protocol::kCpuUsage] =
            QRandomGenerator::global()->bounded(
                0,
                100);

        result[protocol::kMemoryUsage] =
            QRandomGenerator::global()->bounded(
                0,
                100);

        result[protocol::kTemperature] =
            RandomDouble(25.0, 100.0);
    }

    if (size == MessageSize::Long)
    {
        result[protocol::kSystemUpTime] =
            RandomDouble(1.0, 10000.0);

        result[protocol::kPowerSupply] =
            QRandomGenerator::global()->bounded(
                0,
                10);

        result[protocol::kBufferMisses] =
            QRandomGenerator::global()->bounded(
                0,
                100000);

        result[protocol::kActiveTasks] =
            QRandomGenerator::global()->bounded(
                0,
                500);

        result[protocol::kStorageAvailable] =
            QRandomGenerator::global()->bounded(
                10,
                500);

        result[protocol::kActiveProcesses] =
            QRandomGenerator::global()->bounded(
                20,
                300);

        result[protocol::kWarnings] =
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

    result[protocol::kSeverity] =
        protocol::kInfo;

    result[protocol::kMessage] =
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

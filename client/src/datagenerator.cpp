#include "../include/datagenerator.h"

DataGenerator::DataGenerator(
    const QString& client_id,
    QObject* parent)
    : QObject(parent)
    , client_id_(client_id)
{
    generation_timer_.setSingleShot(true);

    connect(&generation_timer_,
            &QTimer::timeout,
            this,
            &DataGenerator::GenerateData);
}

void DataGenerator::Start()
{
    if (active_)
    {
        return;
    }

    active_ = true;

    ScheduleNextGeneration();
}

void DataGenerator::Stop()
{
    active_ = false;

    generation_timer_.stop();
}

bool DataGenerator::IsActive() const
{
    return active_;
}

void DataGenerator::GenerateData()
{
    if (!active_)
    {
        return;
    }

    const int message_type =
        QRandomGenerator::global()->bounded(3);

    QJsonObject object;

    switch (message_type)
    {
    case 0:
    {
        object =
            MessageFactory::CreateNetworkMetrics(
                client_id_);

        break;
    }

    case 1:
    {
        object =
            MessageFactory::CreateDeviceStatus(
                client_id_);

        break;
    }

    default:
    {
        object =
            MessageFactory::CreateLogMessage(
                client_id_);

        break;
    }
    }

    emit DataGenerated(object);

    ScheduleNextGeneration();
}

void DataGenerator::ScheduleNextGeneration()
{
    const int delay_ms =
        QRandomGenerator::global()->bounded(
            10,
            101);

    generation_timer_.start(delay_ms);
}

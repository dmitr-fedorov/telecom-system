#include "../include/datagenerationscheduler.h"

DataGenerationScheduler::DataGenerationScheduler(
    QObject* parent)
    : QObject(parent)
{
    generation_timer_.setSingleShot(true);

    connect(&generation_timer_,
            &QTimer::timeout,
            this,
            &DataGenerationScheduler::generateData);
}

void DataGenerationScheduler::start()
{
    if (active_)
    {
        return;
    }

    active_ = true;

    scheduleNextGeneration();
}

void DataGenerationScheduler::stop()
{
    active_ = false;

    generation_timer_.stop();
}

bool DataGenerationScheduler::isActive() const
{
    return active_;
}

void DataGenerationScheduler::generateData()
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
            DataGenerator::CreateNetworkMetrics();

        break;
    }

    case 1:
    {
        object =
            DataGenerator::CreateDeviceStatus();

        break;
    }

    default:
    {
        object =
            DataGenerator::CreateLogMessage();

        break;
    }
    }

    emit dataGenerated(object);

    scheduleNextGeneration();
}

void DataGenerationScheduler::scheduleNextGeneration()
{
    const int delay_ms =
        QRandomGenerator::global()->bounded(
            10,
            101);

    generation_timer_.start(delay_ms);
}

#include "../include/datagenerationscheduler.h"

DataGenerationScheduler::DataGenerationScheduler(
    QObject* parent)
    : QObject(parent)
{
    generation_timer_.setSingleShot(true);

    connect(&generation_timer_,
            &QTimer::timeout,
            this,
            &DataGenerationScheduler::generateRandomData);
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

void DataGenerationScheduler::generateRandomData()
{
    if (!active_)
    {
        return;
    }

    auto randomData =
        DataGenerator::GenerateRandomData();

    emit dataGenerated(randomData);

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

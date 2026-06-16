#include "../include/datagenerationscheduler.h"

namespace client {

DataGenerationScheduler::DataGenerationScheduler(QObject* parent)
    : QObject(parent) {
  generation_timer_.setSingleShot(true);

  connect(&generation_timer_, &QTimer::timeout, this,
          &DataGenerationScheduler::generateRandomData);
}

void DataGenerationScheduler::start() {
  if (active_) {
    return;
  }

  active_ = true;

  scheduleNextGeneration();

  qInfo() << "Генерация данных начата";
}

void DataGenerationScheduler::stop() {
  if (active_) {
    qInfo() << "Генерация данных остановлена";
  }

  active_ = false;

  generation_timer_.stop();
}

void DataGenerationScheduler::generateRandomData() {
  if (!active_) {
    return;
  }

  auto randomData = DataGenerator::GenerateRandomData();

  emit dataGenerated(randomData);

  scheduleNextGeneration();
}

void DataGenerationScheduler::scheduleNextGeneration() {
  const int delay_ms = QRandomGenerator::global()->bounded(10, 101);

  generation_timer_.start(delay_ms);
}

}  // namespace client

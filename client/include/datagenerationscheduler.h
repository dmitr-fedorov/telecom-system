#pragma once

#include <QJsonObject>
#include <QObject>
#include <QRandomGenerator>
#include <QTimer>

#include "datagenerator.h"

namespace client {

// Генерирует случайные данные
// по таймеру со случайной задержкой от 0.01 до 0.1 секунд.
// Испускает сигнал со сгенерированными данными.
// Типы генерируемых данных: NetworkMetrics, DeviceStatus, Log.
// Размер генерируемых данных: Short, Medium, Long.
class DataGenerationScheduler : public QObject {
  Q_OBJECT

 public:
  explicit DataGenerationScheduler(QObject* parent = nullptr);

  void start();

  void stop();

 signals:
  void dataGenerated(const QJsonObject& object);

 private:
  QTimer generation_timer_;

  bool active_ = false;

  void scheduleNextGeneration();

 private slots:
  void generateRandomData();
};

}  // namespace client

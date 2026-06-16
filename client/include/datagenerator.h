#pragma once

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QString>

#include "../../shared/include/protocol.h"

namespace client {

// Генерирует случайные данные случайного размера в виде QJsonObject.
// Типы генерируемых данных: NetworkMetrics, DeviceStatus, Log.
// Размер генерируемых данных: Short, Medium, Long.
class DataGenerator {
 public:
  static QJsonObject GenerateRandomData();

 private:
  enum class DataSize { Short, Medium, Long };

  static QJsonObject GenerateNetworkMetrics();

  static QJsonObject GenerateDeviceStatus();

  static QJsonObject GenerateLog();

  static DataSize GenerateDataSize();

  static QString GenerateLogText(DataSize size);

  static double RandomDouble(double min, double max);
};

}  // namespace client

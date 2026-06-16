#pragma once

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QString>

#include "../../shared/include/protocol.h"

namespace client {

class DataGenerator {
 public:
  static QJsonObject GenerateRandomData();

 private:
  enum class MessageSize { Short, Medium, Long };

  static QJsonObject GenerateNetworkMetrics();

  static QJsonObject GenerateDeviceStatus();

  static QJsonObject GenerateLog();

  static MessageSize GenerateMessageSize();

  static QString GenerateLogText(MessageSize size);

  static double RandomDouble(double min, double max);
};

}  // namespace client

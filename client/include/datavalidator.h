#pragma once

#include <QJsonObject>

#include "../../shared/include/protocol.h"
#include "../../shared/include/types.h"

namespace client {

// Валидирует сгенерированные данные
// в соответствии с полученной конфигурацией лимитов.
// Формирует лог, если значения превышают лимиты.
class DataValidator : public QObject {
  Q_OBJECT

 public:
  explicit DataValidator(QObject* parent = nullptr);

 public slots:
  void applyLimitsConfig(shared::types::LimitsConfig& config);

  void validate(const QJsonObject& json);

 signals:
  void warningLogReady(const QJsonObject& json);

 private:
  shared::types::LimitsConfig limits_config_;

  bool hasKey(const QJsonObject& json, const char* key) const noexcept;

  void append(QString& message, const QString& name,
              const QString& value) const noexcept;
};

}  // namespace client

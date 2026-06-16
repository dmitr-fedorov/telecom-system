#include "../include/datavalidator.h"

namespace client {

DataValidator::DataValidator(QObject* parent) : QObject(parent) {}

void DataValidator::applyLimitsConfig(shared::types::LimitsConfig& config) {
  limits_config_ = config;
}

void DataValidator::validate(const QJsonObject& json) {
  QString message;

  if (limits_config_.latency && hasKey(json, shared::protocol::k_latency)) {
    auto value = json.value(shared::protocol::k_latency).toDouble();

    if (value > *limits_config_.latency) {
      append(message, shared::protocol::k_latency,
             QString::number(value, 'f', 2));
    }
  }

  if (limits_config_.errors && hasKey(json, shared::protocol::k_errors)) {
    auto value = json.value(shared::protocol::k_errors).toInt();

    if (value > *limits_config_.errors) {
      append(message, shared::protocol::k_errors, QString::number(value));
    }
  }

  if (limits_config_.cpu_usage && hasKey(json, shared::protocol::k_cpu_usage)) {
    auto value = json.value(shared::protocol::k_cpu_usage).toInt();

    if (value > *limits_config_.cpu_usage) {
      append(message, shared::protocol::k_cpu_usage, QString::number(value));
    }
  }

  if (limits_config_.temperature &&
      hasKey(json, shared::protocol::k_temperature)) {
    auto value = json.value(shared::protocol::k_temperature).toDouble();

    if (value > *limits_config_.temperature) {
      append(message, shared::protocol::k_temperature,
             QString::number(value, 'f', 2));
    }
  }

  if (!message.isEmpty()) {
    message.prepend("Generated data exceeds limit: ");

    QJsonObject log_json;
    log_json[shared::protocol::k_type] = shared::protocol::k_log;
    log_json[shared::protocol::k_severity] = shared::protocol::k_warning;
    log_json[shared::protocol::k_message] = message;

    emit warningLogReady(log_json);
  }
}

bool DataValidator::hasKey(const QJsonObject& json,
                           const char* key) const noexcept {
  return json.contains(key) && !json.value(key).isNull();
}

void DataValidator::append(QString& message, const QString& name,
                           const QString& value) const noexcept {
  if (!message.isEmpty()) {
    message += ", ";
  }

  message += name + ":" + value;
}

}  // namespace client

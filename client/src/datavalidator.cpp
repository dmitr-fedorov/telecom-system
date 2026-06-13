#include "../include/datavalidator.h"

DataValidator::DataValidator(QObject* parent)
    : QObject(parent)
{

}

void DataValidator::applyLimitsConfig(
    sharedTypes::LimitsConfig& config)
{
    _limits_config = config;
}

void DataValidator::validate(
    const QJsonObject& json)
{
    QString message;

    if (_limits_config.latency &&
        hasKey(json, protocol::kLatency))
    {
        auto value =
            json.value(protocol::kLatency)
                         .toDouble();

        if (value > *_limits_config.latency)
        {
            append(message,
                   protocol::kLatency,
                   QString::number(value, 'f', 2));
        }
    }

    if (_limits_config.errors &&
        hasKey(json, protocol::kErrors))
    {
        auto value =
            json.value(protocol::kErrors)
                        .toInt();

        if (value > *_limits_config.errors)
        {
            append(message,
                   protocol::kErrors,
                   QString::number(value));
        }
    }

    if (_limits_config.cpu_usage &&
        hasKey(json, protocol::kCpuUsage))
    {
        auto value =
            json.value(protocol::kCpuUsage)
                        .toInt();

        if (value > *_limits_config.cpu_usage)
        {
            append(message,
                   protocol::kCpuUsage,
                   QString::number(value));
        }
    }

    if (_limits_config.temperature &&
        hasKey(json, protocol::kTemperature))
    {
        auto value =
            json.value(protocol::kTemperature)
                           .toDouble();

        if (value > *_limits_config.temperature)
        {
            append(message,
                   protocol::kTemperature,
                   QString::number(value, 'f', 2));
        }
    }

    if (!message.isEmpty())
    {
        message.prepend(
            "Generated data exceeds limit: ");

        QJsonObject log_json;
        log_json[protocol::kType] =
            protocol::kLog;
        log_json[protocol::kSeverity] =
            protocol::kWarning;
        log_json[protocol::kMessage] =
            message;

        emit warningLogReady(log_json);
    }
}

bool DataValidator::hasKey(
    const QJsonObject& json,
    const char* key) const noexcept
{
    return json.contains(key) &&
           !json.value(key).isNull();
}

void DataValidator::append(
    QString& message,
    const QString& name,
    const QString& value) const noexcept
{
    if (!message.isEmpty())
    {
        message += ", ";
    }

    message += name + ":" + value;
}

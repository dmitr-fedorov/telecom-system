#pragma once

#include <QJsonObject>

#include "../../shared/include/protocol.h"
#include "../../shared/include/sharedtypes.h"

class DataValidator : public QObject
{
    Q_OBJECT

public:
    explicit DataValidator(
        QObject* parent = nullptr);

public slots:
    void applyLimitsConfig(
        sharedTypes::LimitsConfig& config);

    void validate(
        const QJsonObject& json);

signals:
    void warningLogReady(
        const QJsonObject& json);

private:
    sharedTypes::LimitsConfig
        _limits_config;

    bool hasKey(
        const QJsonObject& json,
        const char* key) const noexcept;

    void append(
        QString& message,
        const QString& name,
        const QString& value) const noexcept;
};

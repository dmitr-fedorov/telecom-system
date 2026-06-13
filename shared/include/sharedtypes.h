#pragma once

#include <optional>
#include <QMetaType>

namespace sharedTypes
{

struct LimitsConfig
{
    std::optional<double> latency;
    std::optional<int> errors;
    std::optional<int> cpu_usage;
    std::optional<double> temperature;
};

} // namespace sharedTypes

Q_DECLARE_METATYPE(sharedTypes::LimitsConfig)

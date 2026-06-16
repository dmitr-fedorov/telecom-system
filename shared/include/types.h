#pragma once

#include <QMetaType>
#include <optional>

namespace shared {

namespace types {

struct LimitsConfig {
  std::optional<double> latency;
  std::optional<int> errors;
  std::optional<int> cpu_usage;
  std::optional<double> temperature;
};

}  // namespace types

}  // namespace shared

Q_DECLARE_METATYPE(shared::types::LimitsConfig)

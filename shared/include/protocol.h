#pragma once

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QString>

namespace shared {

namespace protocol {

inline constexpr auto k_type = "type";

inline constexpr auto k_ack = "ack";
inline constexpr auto k_start_transmission = "start_transmission";
inline constexpr auto k_stop_transmission = "stop_transmission";
inline constexpr auto k_limits_config = "limits_config";

inline constexpr auto k_network_metrics = "NetworkMetrics";
inline constexpr auto k_device_status = "DeviceStatus";
inline constexpr auto k_log = "Log";

inline constexpr auto k_severity = "severity";
inline constexpr auto k_info = "INFO";
inline constexpr auto k_warning = "WARNING";

inline constexpr auto k_message = "message";

inline constexpr auto k_bandwidth = "bandwidth";
inline constexpr auto k_latency = "latency";
inline constexpr auto k_packet_loss = "packet_loss";
inline constexpr auto k_signal_strength = "signal_strength";
inline constexpr auto k_mtu = "mtu";
inline constexpr auto k_rtt = "rtt";
inline constexpr auto k_link_speed = "link_speed";
inline constexpr auto k_jitter = "jitter";
inline constexpr auto k_throughput = "throughput";
inline constexpr auto k_sent_packets = "sent_packets";
inline constexpr auto k_received_packets = "received_packets";
inline constexpr auto k_errors = "errors";

inline constexpr auto k_uptime = "uptime";
inline constexpr auto k_cpu_usage = "cpu_usage";
inline constexpr auto k_memory_usage = "memory_usage";
inline constexpr auto k_temperature = "temperature";
inline constexpr auto k_system_up_time = "system_up_time";
inline constexpr auto k_power_supply = "power_supply";
inline constexpr auto k_buffer_misses = "buffer_misses";
inline constexpr auto k_active_tasks = "active_tasks";
inline constexpr auto k_storage_available = "storage_available";
inline constexpr auto k_active_processes = "active_processes";
inline constexpr auto k_warnings = "warnings";

inline constexpr auto tcp_packet_delimeter = '\n';

QByteArray Serialize(const QJsonObject& json);

bool Deserialize(const QByteArray& data, QJsonObject* out_json);

}  // namespace protocol

}  // namespace shared

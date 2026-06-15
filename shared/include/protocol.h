#pragma once

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QString>

namespace protocol
{

inline constexpr auto kType = "type";

inline constexpr auto kAck = "ack";
inline constexpr auto kStartTransmission = "start_transmission";
inline constexpr auto kStopTransmission = "stop_transmission";
inline constexpr auto kLimitsConfig = "limits_config";

inline constexpr auto kNetworkMetrics = "NetworkMetrics";
inline constexpr auto kDeviceStatus = "DeviceStatus";
inline constexpr auto kLog = "Log";

inline constexpr auto kSeverity = "severity";
inline constexpr auto kInfo = "INFO";
inline constexpr auto kWarning = "WARNING";

inline constexpr auto kMessage = "message";

inline constexpr auto kBandwidth = "bandwidth";
inline constexpr auto kLatency = "latency";
inline constexpr auto kPacketLoss = "packet_loss";
inline constexpr auto kSignalStrength = "signal_strength";
inline constexpr auto kMtu = "mtu";
inline constexpr auto kRtt = "rtt";
inline constexpr auto kLinkSpeed = "link_speed";
inline constexpr auto kJitter = "jitter";
inline constexpr auto kThroughput = "throughput";
inline constexpr auto kSentPackets = "sent_packets";
inline constexpr auto kReceivedPackets = "received_packets";
inline constexpr auto kErrors = "errors";

inline constexpr auto kUptime = "uptime";
inline constexpr auto kCpuUsage = "cpu_usage";
inline constexpr auto kMemoryUsage = "memory_usage";
inline constexpr auto kTemperature = "temperature";
inline constexpr auto kSystemUpTime = "system_up_time";
inline constexpr auto kPowerSupply = "power_supply";
inline constexpr auto kBufferMisses = "buffer_misses";
inline constexpr auto kActiveTasks = "active_tasks";
inline constexpr auto kStorageAvailable = "storage_available";
inline constexpr auto kActiveProcesses = "active_processes";
inline constexpr auto kWarnings = "warnings";

inline constexpr auto TCP_PACKET_DELIMETER = '\n';

QByteArray Serialize(
    const QJsonObject& json);

bool Deserialize(
    const QByteArray& data,
    QJsonObject* out_json);

} // namespace protocol

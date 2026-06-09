#pragma once

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QString>

namespace protocol
{

inline constexpr auto kType = "type";

inline constexpr auto kAck = "Ack";
inline constexpr auto kStart = "Start";

inline constexpr auto kNetworkMetrics = "NetworkMetrics";
inline constexpr auto kDeviceStatus = "DeviceStatus";
inline constexpr auto kLog = "Log";

inline constexpr auto TCP_PACKET_DELIMETER = '\n';

QByteArray Serialize(const QJsonObject& object);

bool Deserialize(const QByteArray& data, QJsonObject* out_object);

} // namespace protocol

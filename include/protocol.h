#pragma once

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QString>

namespace protocol
{

inline constexpr auto TYPE = "type";
inline constexpr auto CLIENT_ID = "client_id";
inline constexpr auto TIMESTAMP = "timestamp";

inline constexpr auto ACK = "Ack";
inline constexpr auto START = "Start";

inline constexpr auto NETWORKMETRICS = "NetworkMetrics";
inline constexpr auto DEVICESTATUS = "DeviceStatus";
inline constexpr auto LOG = "Log";

inline constexpr auto TCP_PACKET_DELIMETER = '\n';

QByteArray Serialize(const QJsonObject& object);

bool Deserialize(const QByteArray& data, QJsonObject* out_object);

QJsonObject CreateAckMessage(const QString& client_id);

} // namespace protocol

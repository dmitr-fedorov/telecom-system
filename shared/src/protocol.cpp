#include "../include/protocol.h"

namespace shared {

namespace protocol {

QByteArray Serialize(const QJsonObject& json) {
  QJsonDocument document(json);

  QByteArray data = document.toJson(QJsonDocument::Compact);

  data.append(tcp_packet_delimeter);

  return data;
}

bool Deserialize(const QByteArray& data, QJsonObject* out_json) {
  if (out_json == nullptr) {
    return false;
  }

  QJsonParseError error;

  const QJsonDocument document = QJsonDocument::fromJson(data, &error);

  if (error.error != QJsonParseError::NoError) {
    return false;
  }

  if (!document.isObject()) {
    return false;
  }

  *out_json = document.object();

  return true;
}

}  // namespace protocol

}  // namespace shared

#include "../include/protocol.h"

namespace protocol
{

QByteArray Serialize(const QJsonObject& object)
{
    QJsonDocument document(object);

    QByteArray data = document.toJson(QJsonDocument::Compact);

    data.append(TCP_PACKET_DELIMETER);

    return data;
}

bool Deserialize(const QByteArray& data, QJsonObject* out_object)
{
    if (out_object == nullptr)
    {
        return false;
    }

    QJsonParseError error;

    const QJsonDocument document =
        QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError)
    {
        return false;
    }

    if (!document.isObject())
    {
        return false;
    }

    *out_object = document.object();

    return true;
}

QJsonObject CreateAckMessage(const QString& client_id)
{
    QJsonObject object;

    object[TYPE] = ACK;
    object[CLIENT_ID] = client_id;
    object["message"] = "Connection accepted";

    return object;
}

} // namespace protocol

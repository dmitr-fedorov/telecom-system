#pragma once

#include <QDateTime>
#include <QMetaType>

namespace appTypes
{

struct ClientInfo
{
    QString client_id;
    QString ip_address;
    QString state;
};

struct ClientData
{
    QString client_id;
    QString type;
    QString content;
    QDateTime timestamp;
};

} // namespace appTypes

Q_DECLARE_METATYPE(appTypes::ClientInfo)
Q_DECLARE_METATYPE(appTypes::ClientData)

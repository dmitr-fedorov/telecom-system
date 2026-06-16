#pragma once

#include <QDateTime>
#include <QMetaType>

namespace server {

namespace types {

struct ClientInfo {
  QString client_id;
  QString ip_address;
  QString state;
};

struct ClientData {
  QString client_id;
  QString type;
  QString content;
  QDateTime timestamp;
};

}  // namespace types

}  // namespace server

Q_DECLARE_METATYPE(server::types::ClientInfo)
Q_DECLARE_METATYPE(server::types::ClientData)

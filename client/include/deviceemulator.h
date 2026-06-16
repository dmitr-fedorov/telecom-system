#pragma once

#include <QObject>

#include "datagenerationscheduler.h"
#include "datavalidator.h"
#include "tcpclient.h"

namespace client {

class DeviceEmulator : public QObject {
  Q_OBJECT

 public:
  explicit DeviceEmulator(QObject* parent = nullptr);

  void start();

 private:
  TcpClient* tcp_client_ = nullptr;

  DataGenerationScheduler* data_generation_scheduler_ = nullptr;

  DataValidator* data_validator_ = nullptr;
};

}  // namespace client

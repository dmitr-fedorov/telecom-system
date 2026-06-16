#include "../include/deviceemulator.h"

namespace client {

DeviceEmulator::DeviceEmulator(QObject* parent) : QObject(parent) {
  tcp_client_ = new TcpClient(this);

  data_generation_scheduler_ = new DataGenerationScheduler(this);

  data_validator_ = new DataValidator(this);

  connect(tcp_client_, &TcpClient::startTransmissionCommandReceived,
          data_generation_scheduler_, &DataGenerationScheduler::start);

  connect(tcp_client_, &TcpClient::stopTransmissionCommandReceived,
          data_generation_scheduler_, &DataGenerationScheduler::stop);

  connect(tcp_client_, &TcpClient::disconnected, data_generation_scheduler_,
          &DataGenerationScheduler::stop);

  connect(data_generation_scheduler_, &DataGenerationScheduler::dataGenerated,
          tcp_client_, &TcpClient::sendData);

  connect(tcp_client_, &TcpClient::limitsConfigReceived, data_validator_,
          &DataValidator::applyLimitsConfig);

  connect(data_generation_scheduler_, &DataGenerationScheduler::dataGenerated,
          data_validator_, &DataValidator::validate);

  connect(data_validator_, &DataValidator::warningLogReady, tcp_client_,
          &TcpClient::sendData);
}

void DeviceEmulator::start() { tcp_client_->start(); }

}  // namespace client

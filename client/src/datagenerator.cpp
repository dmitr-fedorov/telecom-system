#include "../include/datagenerator.h"

namespace client {

QJsonObject DataGenerator::GenerateRandomData() {
  const auto data_type = QRandomGenerator::global()->bounded(3);

  QJsonObject result;

  switch (data_type) {
    case 0: {
      result = GenerateNetworkMetrics();

      break;
    }

    case 1: {
      result = GenerateDeviceStatus();

      break;
    }

    default: {
      result = GenerateLog();

      break;
    }
  }

  return result;
}

QJsonObject DataGenerator::GenerateNetworkMetrics() {
  const DataSize size = GenerateDataSize();

  QJsonObject result;

  result[shared::protocol::k_type] = shared::protocol::k_network_metrics;

  result[shared::protocol::k_bandwidth] = RandomDouble(10.0, 1000.0);

  if (size >= DataSize::Medium) {
    result[shared::protocol::k_latency] = RandomDouble(1.0, 150.0);

    result[shared::protocol::k_packet_loss] = RandomDouble(0.0, 5.0);

    result[shared::protocol::k_signal_strength] = RandomDouble(40.0, 100.0);
  }

  if (size == DataSize::Long) {
    result[shared::protocol::k_mtu] = RandomDouble(576.0, 9000.0);

    result[shared::protocol::k_rtt] = RandomDouble(10.0, 900000.0);

    result[shared::protocol::k_link_speed] = RandomDouble(100.0, 900000.0);

    result[shared::protocol::k_jitter] =
        QRandomGenerator::global()->bounded(1, 30);

    result[shared::protocol::k_throughput] =
        QRandomGenerator::global()->bounded(10, 1500);

    result[shared::protocol::k_sent_packets] =
        QRandomGenerator::global()->bounded(100000, 500000);

    result[shared::protocol::k_received_packets] =
        QRandomGenerator::global()->bounded(100000, 500000);

    result[shared::protocol::k_errors] =
        QRandomGenerator::global()->bounded(0, 1000);
  }

  return result;
}

QJsonObject DataGenerator::GenerateDeviceStatus() {
  const DataSize size = GenerateDataSize();

  QJsonObject result;

  result[shared::protocol::k_type] = shared::protocol::k_device_status;

  result[shared::protocol::k_uptime] =
      QRandomGenerator::global()->bounded(1000, 1000000);

  if (size >= DataSize::Medium) {
    result[shared::protocol::k_cpu_usage] =
        QRandomGenerator::global()->bounded(0, 100);

    result[shared::protocol::k_memory_usage] =
        QRandomGenerator::global()->bounded(0, 100);

    result[shared::protocol::k_temperature] = RandomDouble(25.0, 100.0);
  }

  if (size == DataSize::Long) {
    result[shared::protocol::k_system_up_time] = RandomDouble(1.0, 10000.0);

    result[shared::protocol::k_power_supply] =
        QRandomGenerator::global()->bounded(0, 10);

    result[shared::protocol::k_buffer_misses] =
        QRandomGenerator::global()->bounded(0, 100000);

    result[shared::protocol::k_active_tasks] =
        QRandomGenerator::global()->bounded(0, 500);

    result[shared::protocol::k_storage_available] =
        QRandomGenerator::global()->bounded(10, 500);

    result[shared::protocol::k_active_processes] =
        QRandomGenerator::global()->bounded(20, 300);

    result[shared::protocol::k_warnings] =
        QRandomGenerator::global()->bounded(0, 10);
  }

  return result;
}

QJsonObject DataGenerator::GenerateLog() {
  const DataSize size = GenerateDataSize();

  QJsonObject result;

  result[shared::protocol::k_type] = shared::protocol::k_log;

  result[shared::protocol::k_severity] = shared::protocol::k_info;

  result[shared::protocol::k_message] = GenerateLogText(size);

  return result;
}

DataGenerator::DataSize DataGenerator::GenerateDataSize() {
  const int value = QRandomGenerator::global()->bounded(3);

  if (value == 0) {
    return DataSize::Short;
  }

  if (value == 1) {
    return DataSize::Medium;
  }

  return DataSize::Long;
}

QString DataGenerator::GenerateLogText(DataSize size) {
  if (size == DataSize::Short) {
    return "Connection restarted";
  }

  if (size == DataSize::Medium) {
    return "Temporary communication issue detected "
           "during data synchronization. "
           "Recovery completed successfully.";
  }

  return "Application detected temporary instability "
         "during data synchronization procedure. "
         "Several reconnect attempts were required "
         "to restore communication with remote "
         "services. Monitoring subsystem reported "
         "increased response time and intermittent "
         "packet processing delays during the "
         "recovery interval.";
}

double DataGenerator::RandomDouble(double min, double max) {
  const int min_int = static_cast<int>(min * 100.0);

  const int max_int = static_cast<int>(max * 100.0);

  return QRandomGenerator::global()->bounded(min_int, max_int) / 100.0;
}

}  // namespace client

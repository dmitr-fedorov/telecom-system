#pragma once

#include <QDateTime>
#include <QHash>
#include <QMainWindow>
#include <QScrollBar>
#include <QTimer>
#include <QVector>

#include "../../shared/include/types.h"
#include "../include/configlimitsdialog.h"
#include "../include/tcpservercontroller.h"
#include "../include/types.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

namespace server {

// Управляет интерфейсом главного окна.
// Инициирует операции TcpServerController,
// отображает полученные от него данные.
class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

 private:
  static constexpr auto start_transmission_text_ = "Начать передачу";
  static constexpr auto stop_transmission_text_ = "Остановить передачу";

  Ui::MainWindow* ui;

  TcpServerController tcp_server_controller_;

  QHash<QString, int> client_info_rows_;

  QTimer table_update_timer_;

  QVector<server::types::ClientData> pending_client_data_;

  ConfigLimitsDialog* config_limits_dialog_ = nullptr;

 private slots:
  void onServerStarted();

  void onClientConnectionStateChanged(const server::types::ClientInfo& info);

  void onStartStopTransmissionClicked();

  void onClientsTransmittingStateChanged(bool transmitting);

  void onEventOccured(const QString& event);

  // Помещает полученные данные во временное хранилище
  void onClientDataReceived(const server::types::ClientData& data);

  // Заносит полученные данные из временного хранилища
  // в таблицу по срабатыванию таймера для повышения производительности
  void addPendingClientData();

  void onConfigLimitsClicked();

  void onLimitsConfigSubmitted(const shared::types::LimitsConfig& config);
};

}  // namespace server

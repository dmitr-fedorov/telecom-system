#include "../include/mainwindow.h"

#include "./ui_mainwindow.h"

namespace server {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  ui->pb_start_stop_transmission->setText(start_transmission_text_);

  ui->pb_start_stop_transmission->setEnabled(false);
  ui->pb_config_limits->setEnabled(false);

  ui->tw_clients_info->setEditTriggers(QAbstractItemView::NoEditTriggers);

  ui->tw_clients_info->setColumnWidth(0, 100);
  ui->tw_clients_info->setColumnWidth(1, 150);
  ui->tw_clients_info->setColumnWidth(2, 150);

  ui->tw_clients_data->setEditTriggers(QAbstractItemView::NoEditTriggers);

  ui->tw_clients_data->setColumnWidth(0, 100);
  ui->tw_clients_data->setColumnWidth(1, 100);
  ui->tw_clients_data->setColumnWidth(2, 700);
  ui->tw_clients_data->setColumnWidth(3, 80);

  config_limits_dialog_ = new ConfigLimitsDialog(this);

  connect(ui->pb_config_limits, &QPushButton::clicked, this,
          &MainWindow::onConfigLimitsClicked);

  connect(config_limits_dialog_, &ConfigLimitsDialog::limitsConfigSubmitted,
          this, &MainWindow::onLimitsConfigSubmitted);

  connect(ui->pb_start_stop_transmission, &QPushButton::clicked, this,
          &MainWindow::onStartStopTransmissionClicked);

  connect(&tcp_server_controller_,
          &TcpServerController::clientConnectionStateChanged, this,
          &MainWindow::onClientConnectionStateChanged);

  connect(&tcp_server_controller_,
          &TcpServerController::clientsTransmittingStateChanged, this,
          &MainWindow::onClientsTransmittingStateChanged);

  connect(&tcp_server_controller_, &TcpServerController::eventOccurred, this,
          &MainWindow::onEventOccured);

  connect(&tcp_server_controller_, &TcpServerController::clientDataReceived,
          this, &MainWindow::onClientDataReceived);

  connect(&tcp_server_controller_, &TcpServerController::serverStarted, this,
          &MainWindow::onServerStarted);

  connect(&table_update_timer_, &QTimer::timeout, this,
          &MainWindow::addPendingRows);

  table_update_timer_.start(300);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::onServerStarted() {
  ui->pb_start_stop_transmission->setEnabled(true);
  ui->pb_config_limits->setEnabled(true);
}

void MainWindow::onClientConnectionStateChanged(
    const server::types::ClientInfo& info) {
  auto it = client_info_rows_.find(info.client_id);

  if (it != client_info_rows_.end()) {
    const int row = *it;

    ui->tw_clients_info->item(row, 2)->setText(info.state);

    return;
  }

  auto* scroll_bar = ui->tw_clients_info->verticalScrollBar();

  const bool scroll_was_at_bottom =
      scroll_bar->value() == scroll_bar->maximum();

  const int row = ui->tw_clients_info->rowCount();

  ui->tw_clients_info->insertRow(row);

  auto* item = new QTableWidgetItem(info.client_id);
  item->setTextAlignment(Qt::AlignCenter);
  ui->tw_clients_info->setItem(row, 0, item);

  item = new QTableWidgetItem(info.ip_address);
  item->setTextAlignment(Qt::AlignCenter);
  ui->tw_clients_info->setItem(row, 1, item);

  item = new QTableWidgetItem(info.state);
  item->setTextAlignment(Qt::AlignCenter);
  ui->tw_clients_info->setItem(row, 2, item);

  client_info_rows_.insert(info.client_id, row);

  if (scroll_was_at_bottom) {
    ui->tw_clients_info->scrollToBottom();
  }
}

void MainWindow::onStartStopTransmissionClicked() {
  if (ui->pb_start_stop_transmission->text() == start_transmission_text_) {
    tcp_server_controller_.startClientsDataTransmission();
  } else {
    tcp_server_controller_.stopClientsDataTransmission();
  }

  ui->pb_start_stop_transmission->setEnabled(false);
}

void MainWindow::onClientsTransmittingStateChanged(bool transmitting) {
  ui->pb_start_stop_transmission->setText(
      transmitting ? stop_transmission_text_ : start_transmission_text_);

  ui->pb_start_stop_transmission->setEnabled(true);
}

void MainWindow::onEventOccured(const QString& event) {
  ui->te_events->append(event);
}

void MainWindow::onClientDataReceived(const server::types::ClientData& data) {
  pending_client_data_.push_back(data);
}

void MainWindow::addPendingRows() {
  if (pending_client_data_.isEmpty()) {
    return;
  }

  auto* scroll_bar = ui->tw_clients_data->verticalScrollBar();

  const bool scroll_was_at_bottom =
      scroll_bar->value() == scroll_bar->maximum();

  ui->tw_clients_data->setUpdatesEnabled(false);

  const auto old_row_count = ui->tw_clients_data->rowCount();
  ui->tw_clients_data->setRowCount(old_row_count + pending_client_data_.size());

  auto row = old_row_count;

  for (const auto& data : std::as_const(pending_client_data_)) {
    auto* item = new QTableWidgetItem(data.client_id);
    item->setTextAlignment(Qt::AlignCenter);
    ui->tw_clients_data->setItem(row, 0, item);

    item = new QTableWidgetItem(data.type);
    item->setTextAlignment(Qt::AlignCenter);
    ui->tw_clients_data->setItem(row, 1, item);

    ui->tw_clients_data->setItem(row, 2, new QTableWidgetItem(data.content));

    item = new QTableWidgetItem(data.timestamp.toString("HH:mm:ss"));
    item->setTextAlignment(Qt::AlignCenter);
    ui->tw_clients_data->setItem(row, 3, item);

    ++row;
  }

  pending_client_data_.clear();

  ui->tw_clients_data->setUpdatesEnabled(true);

  if (scroll_was_at_bottom) {
    ui->tw_clients_data->scrollToBottom();
  }
}

void MainWindow::onConfigLimitsClicked() { config_limits_dialog_->exec(); }

void MainWindow::onLimitsConfigSubmitted(
    const shared::types::LimitsConfig& config) {
  tcp_server_controller_.applyLimitsConfig(config);
}

}  // namespace server

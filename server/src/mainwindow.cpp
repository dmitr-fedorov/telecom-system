#include "../include/mainwindow.h"
#include "./ui_mainwindow.h"

const QString MainWindow::START_TEXT = "Начать передачу";
const QString MainWindow::STOP_TEXT = "Остановить передачу";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pb_start_stop->setText(START_TEXT);

    ui->pb_start_stop->setEnabled(false);
    ui->pb_config_limits->setEnabled(false);

    ui->tw_clients_info->setEditTriggers(
        QAbstractItemView::NoEditTriggers);

    ui->tw_clients_info->setColumnWidth(0, 100);
    ui->tw_clients_info->setColumnWidth(1, 150);
    ui->tw_clients_info->setColumnWidth(2, 150);

    ui->tw_clients_data->setEditTriggers(
        QAbstractItemView::NoEditTriggers);

    ui->tw_clients_data->setColumnWidth(0, 100);
    ui->tw_clients_data->setColumnWidth(1, 100);
    ui->tw_clients_data->setColumnWidth(2, 700);
    ui->tw_clients_data->setColumnWidth(3, 80);

    _config_limits_dialog =
        new ConfigLimitsDialog(this);

    connect(ui->pb_config_limits,
            &QPushButton::clicked,
            this,
            &MainWindow::onConfigLimitsClicked);

    connect(_config_limits_dialog,
            &ConfigLimitsDialog::limitsConfigSubmitted,
            this,
            &MainWindow::onLimitsConfigSubmitted);

    connect(ui->pb_start_stop,
            &QPushButton::clicked,
            this,
            &MainWindow::onClientsStartStopClicked);

    connect(&_tcp_server_controller,
            &TcpServerController::clientConnectionStateChanged,
            this,
            &MainWindow::onClientConnectionStateChanged);

    connect(&_tcp_server_controller,
            &TcpServerController::clientsRunningStateChanged,
            this,
            &MainWindow::onClientsRunningStateChanged);

    connect(&_tcp_server_controller,
            &TcpServerController::eventOccurred,
            this,
            &MainWindow::onEventOccured);

    connect(&_tcp_server_controller,
            &TcpServerController::clientDataReceived,
            this,
            &MainWindow::onClientDataReceived);

    connect(&_tcp_server_controller,
            &TcpServerController::serverStarted,
            this,
            &MainWindow::onServerStarted);

    connect(&_update_timer,
             &QTimer::timeout,
             this,
             &MainWindow::addPendingRows);

    _update_timer.start(300);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onServerStarted()
{
    ui->pb_start_stop->setEnabled(true);
    ui->pb_config_limits->setEnabled(true);
}

void MainWindow::onClientConnectionStateChanged(
    const appTypes::ClientInfo& info)
{
    auto it =
        _client_info_rows.find(info.client_id);

    if (it != _client_info_rows.end())
    {
        const int row = *it;

        ui->tw_clients_info->item(row, 2)
            ->setText(info.state);

        return;
    }

    auto* scroll_bar =
        ui->tw_clients_info->verticalScrollBar();

    const bool scroll_was_at_bottom =
        scroll_bar->value() ==
        scroll_bar->maximum();

    const int row =
        ui->tw_clients_info->rowCount();

    ui->tw_clients_info->insertRow(row);

    auto* item =
        new QTableWidgetItem(info.client_id);

    item->setTextAlignment(Qt::AlignCenter);

    ui->tw_clients_info->setItem(
        row,
        0,
        item);

    item =
        new QTableWidgetItem(info.ip_address);

    item->setTextAlignment(Qt::AlignCenter);

    ui->tw_clients_info->setItem(
        row,
        1,
        item);

    item =
        new QTableWidgetItem(info.state);

    item->setTextAlignment(Qt::AlignCenter);

    ui->tw_clients_info->setItem(
        row,
        2,
        item);

    _client_info_rows.insert(
        info.client_id,
        row);

    if (scroll_was_at_bottom)
    {
        ui->tw_clients_info->scrollToBottom();
    }
}

void MainWindow::onClientsStartStopClicked()
{
    if (ui->pb_start_stop->text() == START_TEXT)
    {
        _tcp_server_controller.startClients();
    }
    else
    {
        _tcp_server_controller.stopClients();
    }

    ui->pb_start_stop->setEnabled(false);
}

void MainWindow::onClientsRunningStateChanged(bool running)
{
    ui->pb_start_stop->setText(
        running ? STOP_TEXT : START_TEXT);

    ui->pb_start_stop->setEnabled(true);
}

void MainWindow::onEventOccured(const QString& event)
{
    ui->te_events->append(event);
}

void MainWindow::onClientDataReceived(
    const appTypes::ClientData& data)
{
    _pending_data_rows.push_back(data);
}

void MainWindow::addPendingRows()
{
    if (_pending_data_rows.isEmpty())
    {
        return;
    }

    auto* scroll_bar =
        ui->tw_clients_data->verticalScrollBar();

    const bool scroll_was_at_bottom =
        scroll_bar->value() ==
        scroll_bar->maximum();

    ui->tw_clients_data->setUpdatesEnabled(false);

    const auto old_row_count =
        ui->tw_clients_data->rowCount();

    ui->tw_clients_data->setRowCount(
        old_row_count + _pending_data_rows.size());

    auto row = old_row_count;

    for (const auto& data :
         std::as_const(_pending_data_rows))
    {
        auto* item =
            new QTableWidgetItem(data.client_id);

        item->setTextAlignment(
            Qt::AlignCenter);

        ui->tw_clients_data->setItem(
            row, 0, item);

        item =
            new QTableWidgetItem(data.type);

        item->setTextAlignment(
            Qt::AlignCenter);

        ui->tw_clients_data->setItem(
            row, 1, item);

        ui->tw_clients_data->setItem(
            row,
            2,
            new QTableWidgetItem(data.content));

        item =
            new QTableWidgetItem(
            data.timestamp.toString("HH:mm:ss"));

        item->setTextAlignment(
            Qt::AlignCenter);

        ui->tw_clients_data->setItem(
            row, 3, item);

        ++row;
    }

    _pending_data_rows.clear();

    ui->tw_clients_data->setUpdatesEnabled(true);

    if (scroll_was_at_bottom)
    {
        ui->tw_clients_data->scrollToBottom();
    }
}

void MainWindow::onConfigLimitsClicked()
{
    _config_limits_dialog->exec();
}

void MainWindow::onLimitsConfigSubmitted(
    const sharedTypes::LimitsConfig& config)
{
    _tcp_server_controller
        .applyLimitsConfig(config);
}

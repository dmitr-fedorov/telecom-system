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

    _configLimitsDialog =
        new ConfigLimitsDialog(this);

    connect(ui->pb_config_limits,
            &QPushButton::clicked,
            this,
            &MainWindow::onConfigLimitsClicked);

    connect(_configLimitsDialog,
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
    const QString& client_id,
    const QString& ip_address,
    const QString& state)
{
    auto it = _clientRows.find(client_id);

    if (it != _clientRows.end())
    {
        const int row = *it;

        ui->tw_clients_info->item(row, 2)
            ->setText(state);

        return;
    }

    const int row =
        ui->tw_clients_info->rowCount();

    ui->tw_clients_info->insertRow(row);

    auto* item = new QTableWidgetItem(client_id);
    item->setTextAlignment(Qt::AlignCenter);

    ui->tw_clients_info->setItem(
        row,
        0,
        item);

    item = new QTableWidgetItem(ip_address);
    item->setTextAlignment(Qt::AlignCenter);

    ui->tw_clients_info->setItem(
        row,
        1,
        item);

    item = new QTableWidgetItem(state);
    item->setTextAlignment(Qt::AlignCenter);

    ui->tw_clients_info->setItem(
        row,
        2,
        item);

    _clientRows.insert(
        client_id,
        row);
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
    const QString& clientId,
    const QString& type,
    const QString& content,
    const QDateTime& timestamp)
{
    const int row =
        ui->tw_clients_data->rowCount();

    ui->tw_clients_data->insertRow(row);

    auto* item = new QTableWidgetItem(clientId);
    item->setTextAlignment(Qt::AlignCenter);

    ui->tw_clients_data->setItem(
        row,
        0,
        item);

    item = new QTableWidgetItem(type);
    item->setTextAlignment(Qt::AlignCenter);

    ui->tw_clients_data->setItem(
        row,
        1,
        item);

    ui->tw_clients_data->setItem(
        row,
        2,
        new QTableWidgetItem(content));

    item = new QTableWidgetItem(
        timestamp.toString("HH:mm:ss"));
    item->setTextAlignment(Qt::AlignCenter);

    ui->tw_clients_data->setItem(
        row,
        3,
        item);
}

void MainWindow::onConfigLimitsClicked()
{
    _configLimitsDialog->exec();
}

void MainWindow::onLimitsConfigSubmitted(
    const sharedTypes::LimitsConfig& config)
{
    _tcp_server_controller
        .applyLimitsConfig(config);
}

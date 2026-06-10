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

    connect(ui->pb_start_stop,
            &QPushButton::clicked,
            this,
            &MainWindow::onClientsStartStopClicked);

    connect(ui->pb_config_limits,
            &QPushButton::clicked,
            this,
            [this]()
            {
                _server_controller.applyConfiguration(0);
            });

    connect(&_server_controller,
            &TcpServerController::clientConnectionStateChanged,
            this,
            &MainWindow::onClientConnectionStateChanged);

    connect(&_server_controller,
            &TcpServerController::clientsRunningStateChanged,
            this,
            &MainWindow::onClientsRunningStateChanged);

    connect(&_server_controller, &TcpServerController::eventOccurred,
            this, &MainWindow::onEventOccured);

    connect(&_server_controller, &TcpServerController::clientDataReceived,
            this, &MainWindow::onClientDataReceived);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    }

    const int row =
        ui->tw_clients_info->rowCount();

    ui->tw_clients_info->insertRow(row);

    ui->tw_clients_info->setItem(
        row,
        0,
        new QTableWidgetItem(client_id));

    ui->tw_clients_info->setItem(
        row,
        1,
        new QTableWidgetItem(ip_address));

    ui->tw_clients_info->setItem(
        row,
        2,
        new QTableWidgetItem(state));

    _clientRows.insert(
        client_id,
        row);
}

void MainWindow::onClientsStartStopClicked()
{
    if (ui->pb_start_stop->text() == START_TEXT)
    {
        _server_controller.startClients();
    }
    else
    {
        _server_controller.stopClients();
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

    ui->tw_clients_data->setItem(
        row,
        0,
        new QTableWidgetItem(clientId));

    ui->tw_clients_data->setItem(
        row,
        1,
        new QTableWidgetItem(type));

    ui->tw_clients_data->setItem(
        row,
        2,
        new QTableWidgetItem(content));

    ui->tw_clients_data->setItem(
        row,
        3,
        new QTableWidgetItem(
            timestamp.toString("HH:mm:ss")));
}

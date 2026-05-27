#include "../include/mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pb_start_stop,
            &QPushButton::clicked,
            this,
            &MainWindow::onClientsStartStopClicked);

    connect(ui->pb_config,
            &QPushButton::clicked,
            this,
            [this]()
            {
                _server_controller.applyConfiguration(
                    ui->sb_config->value());
            });

    connect(&_server_controller, &ServerController::eventOccurred,
            this, &MainWindow::onEventOccured);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onClientsStartStopClicked()
{
    if (!_is_clients_sending)
    {
        ui->pb_start_stop->setText("Стоп");

        _server_controller.startClients();
    }
    else
    {
        ui->pb_start_stop->setText("Старт");

       _server_controller.stopClients();
    }

    _is_clients_sending = !_is_clients_sending;
}

void MainWindow::onEventOccured(const QString& event)
{
    ui->te_events->append(event);
}

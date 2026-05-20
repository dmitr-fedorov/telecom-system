#include "../include/mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pb_start_stop,
            &QPushButton::clicked,
            &_server_controller,
            &ServerController::StartStopClients);

    connect(ui->pb_config,
            &QPushButton::clicked,
            this,
            [this]()
            {
                _server_controller.ApplyConfiguration(
                    ui->sb_config->value());
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}

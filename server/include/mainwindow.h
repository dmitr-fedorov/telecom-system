#pragma once

#include <QMainWindow>

#include "../include/servercontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    ServerController _server_controller;

    bool _is_clients_sending = false;

private slots:
    void onClientsStartStopClicked();

    void onEventOccured(const QString& event);
};

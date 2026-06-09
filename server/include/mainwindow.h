#pragma once

#include <QDateTime>
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
    static const QString START_TEXT;
    static const QString STOP_TEXT;

    Ui::MainWindow *ui;

    ServerController _server_controller;

private slots:
    void onClientsStartStopClicked();

    void onClientsRunningStateChanged(bool running);

    void onEventOccured(const QString& event);

    void onClientDataReceived(
        const QString& clientId,
        const QString& type,
        const QString& content,
        const QDateTime& timestamp);
};

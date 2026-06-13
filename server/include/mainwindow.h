#pragma once

#include <QDateTime>
#include <QHash>
#include <QMainWindow>

#include "../include/tcpservercontroller.h"
#include "../include/configlimitsdialog.h"
#include "../../shared/include/sharedtypes.h"

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

    TcpServerController _tcp_server_controller;

    QHash<QString, int> _clientRows;

    ConfigLimitsDialog* _configLimitsDialog = nullptr;

private slots:
    void onServerStarted();

    void onClientConnectionStateChanged(
        const QString& client_id,
        const QString& ip_address,
        const QString& state);

    void onClientsStartStopClicked();

    void onClientsRunningStateChanged(bool running);

    void onEventOccured(const QString& event);

    void onClientDataReceived(
        const QString& clientId,
        const QString& type,
        const QString& content,
        const QDateTime& timestamp);

    void onConfigLimitsClicked();

    void onLimitsConfigSubmitted(
        const sharedTypes::LimitsConfig& config);
};

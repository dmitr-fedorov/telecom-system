#pragma once

#include <QDateTime>
#include <QHash>
#include <QVector>
#include <QMainWindow>
#include <QScrollBar>
#include <QTimer>

#include "../include/apptypes.h"
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

    QHash<QString, int> _client_info_rows;

    QTimer _update_timer;

    QVector<appTypes::ClientData> _pending_data_rows;

    ConfigLimitsDialog* _config_limits_dialog = nullptr;

private slots:
    void onServerStarted();

    void onClientConnectionStateChanged(
        const appTypes::ClientInfo& info);

    void onClientsStartStopClicked();

    void onClientsRunningStateChanged(bool running);

    void onEventOccured(const QString& event);

    void onClientDataReceived(
        const appTypes::ClientData& data);

    void addPendingRows();

    void onConfigLimitsClicked();

    void onLimitsConfigSubmitted(
        const sharedTypes::LimitsConfig& config);
};

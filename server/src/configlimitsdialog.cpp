#include "../include/configlimitsdialog.h"
#include "./ui_configlimitsdialog.h"

ConfigLimitsDialog::ConfigLimitsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LimitsConfigDialog)
{
    ui->setupUi(this);

    ui->dsb_latency->setDecimals(2);
    ui->dsb_temperature->setDecimals(2);

    ui->dsb_latency->setEnabled(false);
    ui->sb_errors->setEnabled(false);
    ui->sb_cpu_usage->setEnabled(false);
    ui->dsb_temperature->setEnabled(false);

    connect(ui->cb_latency,
            &QCheckBox::toggled,
            ui->dsb_latency,
            &QDoubleSpinBox::setEnabled);

    connect(ui->cb_errors,
            &QCheckBox::toggled,
            ui->sb_errors,
            &QSpinBox::setEnabled);

    connect(ui->cb_cpu_usage,
            &QCheckBox::toggled,
            ui->sb_cpu_usage,
            &QSpinBox::setEnabled);

    connect(ui->cb_temperature,
            &QCheckBox::toggled,
            ui->dsb_temperature,
            &QSpinBox::setEnabled);
}

ConfigLimitsDialog::~ConfigLimitsDialog()
{
    delete ui;
}

void ConfigLimitsDialog::accept()
{
    sharedTypes::LimitsConfig config;

    if (ui->dsb_latency->isEnabled())
    {
        config.latency =
            ui->dsb_latency->value();
    }

    if (ui->sb_errors->isEnabled())
    {
        config.errors =
            ui->sb_errors->value();
    }

    if (ui->sb_cpu_usage->isEnabled())
    {
        config.cpu_usage =
            ui->sb_cpu_usage->value();
    }

    if (ui->dsb_temperature->isEnabled())
    {
        config.temperature =
            ui->dsb_temperature->value();
    }

    emit limitsConfigSubmitted(config);

    QDialog::accept();
}

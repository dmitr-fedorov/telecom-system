#include "../include/configlimitsdialog.h"

#include "./ui_configlimitsdialog.h"

namespace server {

ConfigLimitsDialog::ConfigLimitsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ConfigLimitsDialog) {
  ui->setupUi(this);

  ui->dsb_latency->setDecimals(2);
  ui->dsb_temperature->setDecimals(2);

  ui->dsb_latency->setEnabled(false);
  ui->sb_errors->setEnabled(false);
  ui->sb_cpu_usage->setEnabled(false);
  ui->dsb_temperature->setEnabled(false);

  connect(ui->cb_latency, &QCheckBox::toggled, ui->dsb_latency,
          &QDoubleSpinBox::setEnabled);

  connect(ui->cb_errors, &QCheckBox::toggled, ui->sb_errors,
          &QSpinBox::setEnabled);

  connect(ui->cb_cpu_usage, &QCheckBox::toggled, ui->sb_cpu_usage,
          &QSpinBox::setEnabled);

  connect(ui->cb_temperature, &QCheckBox::toggled, ui->dsb_temperature,
          &QSpinBox::setEnabled);
}

ConfigLimitsDialog::~ConfigLimitsDialog() { delete ui; }

void ConfigLimitsDialog::setConfig(const shared::types::LimitsConfig &config) {
  ui->cb_latency->setChecked(config.latency.has_value());
  ui->dsb_latency->setValue(config.latency.value_or(0.0));

  ui->cb_errors->setChecked(config.errors.has_value());
  ui->sb_errors->setValue(config.errors.value_or(0));

  ui->cb_cpu_usage->setChecked(config.cpu_usage.has_value());
  ui->sb_cpu_usage->setValue(config.cpu_usage.value_or(0));

  ui->cb_temperature->setChecked(config.temperature.has_value());
  ui->dsb_temperature->setValue(config.temperature.value_or(0.0));
}

void ConfigLimitsDialog::accept() {
  shared::types::LimitsConfig config;

  if (ui->dsb_latency->isEnabled()) {
    config.latency = ui->dsb_latency->value();
  }

  if (ui->sb_errors->isEnabled()) {
    config.errors = ui->sb_errors->value();
  }

  if (ui->sb_cpu_usage->isEnabled()) {
    config.cpu_usage = ui->sb_cpu_usage->value();
  }

  if (ui->dsb_temperature->isEnabled()) {
    config.temperature = ui->dsb_temperature->value();
  }

  emit limitsConfigSubmitted(config);

  QDialog::accept();
}

}  // namespace server

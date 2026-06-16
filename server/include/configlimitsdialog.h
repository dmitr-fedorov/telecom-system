#pragma once

#include <QDialog>

#include "../../shared/include/types.h"

namespace Ui {
class ConfigLimitsDialog;
}

namespace server {

// Окно для настройки лимитов значений, принимаемых от клиентов
class ConfigLimitsDialog : public QDialog {
  Q_OBJECT

 public:
  explicit ConfigLimitsDialog(QWidget *parent = nullptr);

  ~ConfigLimitsDialog();

 signals:
  void limitsConfigSubmitted(const shared::types::LimitsConfig &config);

 protected:
  void accept() override;

 private:
  Ui::ConfigLimitsDialog *ui;
};

}  // namespace server

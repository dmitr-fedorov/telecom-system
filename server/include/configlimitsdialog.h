#pragma once

#include <QDialog>

#include "../../shared/include/sharedtypes.h"

namespace Ui {
class LimitsConfigDialog;
}

class ConfigLimitsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigLimitsDialog(
        QWidget *parent = nullptr);

    ~ConfigLimitsDialog();

signals:
    void limitsConfigSubmitted(
        const sharedTypes::LimitsConfig& config);

protected:
    void accept() override;

private:
    Ui::LimitsConfigDialog *ui;
};

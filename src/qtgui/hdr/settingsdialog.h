#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

private slots:
    void on_comboBox_config_preset_currentIndexChanged(int index);

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H

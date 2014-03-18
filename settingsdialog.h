#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

class MainWindow;

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(MainWindow *mainWin, QWidget *parent = 0);
    ~SettingsDialog();

private slots:
    void on_buttonBox_accepted();

    void on_tSpinBox_valueChanged(int arg1);

    void on_mSpinBox_valueChanged(int arg1);

    void on_presetsComboBox_currentIndexChanged(int index);

private:
    Ui::SettingsDialog *ui;
    MainWindow *mainWin;

    void InitSettingsUI();
    void DisplayBchInfo(int m, int t);

    bool preset_changed[2];
};

#endif // SETTINGSDIALOG_H

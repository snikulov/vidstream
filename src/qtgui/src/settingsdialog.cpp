#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

SettingsDialog::SettingsDialog(cfg_ptr_t cfg, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    cfg_(cfg)
{
    ui->setupUi(this);
    ui_update(*ui, *cfg_);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;

    // do not delete cfg_ it is just reflection from main app
}

void SettingsDialog::on_comboBox_config_preset_currentIndexChanged(int index)
{

}

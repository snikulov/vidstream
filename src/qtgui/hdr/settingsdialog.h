#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

#include "config_iface.hpp"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();


    explicit SettingsDialog(cfg_ptr_t cfg,
            QWidget * parent = 0);

private slots:
    void on_comboBox_config_preset_currentIndexChanged(int index);

private:
    Ui::SettingsDialog *ui;
    cfg_ptr_t cfg_;
};

bool ui_update(Ui::SettingsDialog &u, const boost::property_tree::ptree &cfg);


#if 0
void update_stat(Ui::MainWindow & u, const std::string& data);

#endif

void cfg_update(boost::property_tree::ptree &cfg, const Ui::SettingsDialog &u);


int ui_set_resolution_index(Ui::SettingsDialog& u, const boost::property_tree::ptree& cfg);


int cfg_set_resolution_by_list_index(boost::property_tree::ptree &cfg, int idx);


int ui_set_bch_preset_list_index(Ui::SettingsDialog &u
                                 , const boost::property_tree::ptree &cfg);


int cfg_set_bch_values_by_list_index(boost::property_tree::ptree& cfg
                                     , Ui::SettingsDialog & u, int idx);

#endif // SETTINGSDIALOG_H

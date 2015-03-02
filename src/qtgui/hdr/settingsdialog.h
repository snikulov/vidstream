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
    ~SettingsDialog();

    explicit SettingsDialog(cfg_ptr_t cfg,
            QWidget * parent = 0);

private slots:

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_checkBox_is_gray_stateChanged(int arg1);

    void on_comboBox_camera_resolution_currentIndexChanged(int index);

    void on_spinBox_rst_num_valueChanged(int arg1);

    void on_spinBox_port_cmd_valueChanged(int arg1);

    void on_spinBox_port_data_valueChanged(int arg1);

    void on_comboBox_bch_mode_currentIndexChanged(int index);

    void on_spinBox_bch_m_valueChanged(int arg1);

    void on_spinBox_bch_t_valueChanged(int arg1);

    void on_doubleSpinBox_error_persent_valueChanged(double arg1);

    void on_spinBox_bch_m_editingFinished();

    void on_spinBox_bw_valueChanged(int arg1);

    void on_spinBox_jpeg_quality_valueChanged(int arg1);

    void on_spinBox_fps_limit_valueChanged(int arg1);

    void on_comboBox_ip_selector_currentIndexChanged(int index);

    void on_comboBox_config_preset_currentIndexChanged(int index);

private:
    explicit SettingsDialog(QWidget *parent = 0);

    Ui::SettingsDialog *ui;
    cfg_ptr_t cfg_;
};

bool ui_update(Ui::SettingsDialog &u, const boost::property_tree::ptree &cfg);


void cfg_update(boost::property_tree::ptree &cfg, const Ui::SettingsDialog &u);


int ui_set_resolution_index(Ui::SettingsDialog& u, const boost::property_tree::ptree& cfg);


int cfg_set_resolution_by_list_index(boost::property_tree::ptree &cfg, int idx);


int ui_set_bch_preset_list_index(Ui::SettingsDialog &u
                                 , const boost::property_tree::ptree &cfg);


int cfg_set_bch_values_by_list_index(boost::property_tree::ptree& cfg
                                     , Ui::SettingsDialog & u, int idx);

#endif // SETTINGSDIALOG_H

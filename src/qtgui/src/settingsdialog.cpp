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

    // check-point for old values
    write_config_file(cfg_);

    (void)ui_update(*ui, *cfg_);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;

    // do not delete cfg_ it is just reflection from main app
}

void SettingsDialog::on_buttonBox_accepted()
{
    // store configuration
    write_config_file(cfg_);
}

void SettingsDialog::on_buttonBox_rejected()
{
    // drop all chanegs
    read_config_file(cfg_);
}

void SettingsDialog::on_checkBox_is_gray_stateChanged(int arg1)
{
    // grayscale toggled
    bool val = arg1 ? true : false;
    cfg_->put("cfg.img.bw", val);
}

void SettingsDialog::on_comboBox_camera_resolution_currentIndexChanged(int index)
{
    cfg_set_resolution_by_list_index(*cfg_, index);
}

void SettingsDialog::on_spinBox_rst_num_valueChanged(int arg1)
{
    // rst value
    cfg_->put("cfg.img.rst", arg1);
}

void SettingsDialog::on_spinBox_port_cmd_valueChanged(int arg1)
{
    // control port
    cfg_->put("cfg.cmdport", arg1);
}

void SettingsDialog::on_spinBox_port_data_valueChanged(int arg1)
{
    // data port
    cfg_->put("cfg.dataport", arg1);
}

void SettingsDialog::on_comboBox_bch_mode_currentIndexChanged(int index)
{
    // bch preset changed
//    std::cerr << "bch prameters changed: " << index << std::endl;
    cfg_set_bch_values_by_list_index(*cfg_, *ui, index);

}

void SettingsDialog::on_spinBox_bch_m_valueChanged(int arg1)
{
    // bch m value

    ui_set_bch_preset_list_index(*ui, *cfg_);
    if (0 == arg1 || (arg1 >= 5 && arg1 <= 15))
    {
        cfg_->put("cfg.bch.n", arg1);
    }
}

void SettingsDialog::on_spinBox_bch_t_valueChanged(int arg1)
{
    // bch t value
    cfg_->put("cfg.bch.t", arg1);
    ui_set_bch_preset_list_index(*ui, *cfg_);
}

void SettingsDialog::on_doubleSpinBox_error_persent_valueChanged(double arg1)
{
    // changed error value
    cfg_->put("cfg.error.val", arg1);
}

void SettingsDialog::on_spinBox_bch_m_editingFinished()
{

}

void SettingsDialog::on_spinBox_bw_valueChanged(int arg1)
{
    cfg_->put("cfg.bw", arg1);
}

void SettingsDialog::on_spinBox_jpeg_quality_valueChanged(int arg1)
{
    cfg_->put("cfg.img.q", arg1);
}

void SettingsDialog::on_spinBox_fps_limit_valueChanged(int arg1)
{
    cfg_->put("cfg.fps.lim", arg1);
}

void SettingsDialog::on_comboBox_config_preset_currentIndexChanged(int index)
{
    struct profile
    {
        int bw;
        int fps;
        int jpeg_quality;
        int bch_idx;
        int res_idx;
    };

    const struct profile presets[] = {
        {10, 25, 60, 3, 2},
        {10, 25, 40, 1, 2},
        {10, 20, 30, 2, 2},
        {5,  15, 40, 2, 2},
        {5,  25, 40, 1, 3}
    };

    const size_t PROFILE_SIZE = sizeof(presets)/sizeof(presets[0]);

    if (index > 0 && index <= PROFILE_SIZE)
    {
        int i = index-1;
        ui->spinBox_bw->setValue(presets[i].bw);
        ui->spinBox_fps_limit->setValue(presets[i].fps);
        ui->spinBox_jpeg_quality->setValue(presets[i].jpeg_quality);
        ui->comboBox_bch_mode->setCurrentIndex(presets[i].bch_idx);
        ui->comboBox_camera_resolution->setCurrentIndex(presets[i].res_idx);
    }

}

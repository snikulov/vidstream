#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QNetworkInterface>
#include <QHostAddress>


SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

SettingsDialog::SettingsDialog(cfg_ptr_t cfg, boost::shared_ptr<profile_list_t> plist, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    cfg_(cfg)
  , plist_(plist)
{
    ui->setupUi(this);

    if(!ui->comboBox_ip_selector->count())
    {
        // get list of interfaces
        foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
        {
            if (address.protocol() == QAbstractSocket::IPv4Protocol)
            {
                QString ip = address.toString();
                ui->comboBox_ip_selector->addItem(ip);
            }
        }
        cfg_->put("cfg.ip",
                  ui->comboBox_ip_selector->itemText(0).toUtf8().constData());
    }

    if (plist_)
    {
        for(size_t i = 0; i < plist_->size(); ++i)
        {
            QString pname = QString::fromUtf8((*plist_)[i].name_.c_str());
            ui->comboBox_config_preset->addItem(pname);
        }
    }

    // check-point for old values
    write_config_file(cfg_, CONFIG_FILE_NAME);

    (void)ui_update(*ui, *cfg_);
    profile p(""
              , cfg_->get<int>("cfg.bw")
              , cfg_->get<int>("cfg.fps.lim")
              , cfg_->get<int>("cfg.img.q")
              , find_index(get_bch_mode_list(), std::make_pair(cfg_->get<int>("cfg.bch.n"), cfg_->get<int>("cfg.bch.t")))
              , find_index(get_resolution_list(), std::make_pair(cfg_->get<int>("cfg.img.width"), cfg_->get<int>("cfg.img.height")))
              );

    int idx = find_index(plist_, p);
    idx++;
    ui->comboBox_config_preset->setCurrentIndex(idx);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;

    // do not delete cfg_ it is just reflection from main app
}

void SettingsDialog::on_buttonBox_accepted()
{
    // store configuration
    write_config_file(cfg_, CONFIG_FILE_NAME);
}

void SettingsDialog::on_buttonBox_rejected()
{
    // drop all chanegs
    read_config_file(cfg_, CONFIG_FILE_NAME);
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
    if (plist_)
    {
        const profile_list_t& presets = *plist_;

        size_t i = static_cast<size_t>(index);

        if (i > 0 && i <= presets.size())
        {
            // correcting i to point profile in vector
            i--;

            ui->spinBox_bw->setValue(presets[i].bw_);
            ui->spinBox_fps_limit->setValue(presets[i].fps_);
            ui->spinBox_jpeg_quality->setValue(presets[i].jpeg_quality_);
            ui->comboBox_bch_mode->setCurrentIndex(presets[i].bch_idx_);
            ui->comboBox_camera_resolution->setCurrentIndex(presets[i].res_idx_);
        }
    }
}

void SettingsDialog::on_comboBox_ip_selector_currentIndexChanged(int index)
{
    cfg_->put("cfg.ip",
              ui->comboBox_ip_selector->itemText(index).toUtf8().constData());
}



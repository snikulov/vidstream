#include "config_iface.hpp"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

typedef struct
{
    int one;
    int two;
} param_pair_t;

static const param_pair_t res_modes[] =
{
    {1280,800}
    ,{1024,780}
    ,{640,480}
    ,{320,240}
};
const size_t RES_MODE_MAX = sizeof(res_modes)/sizeof(res_modes[0]);

static const param_pair_t bch_modes[] =
{
    {0,0}
    ,{5,3}
    ,{5,4}
    ,{7,5}
};
const size_t BCH_PRESET_MAX = sizeof(bch_modes)/sizeof(bch_modes[0]);

bool ui_update(Ui::SettingsDialog &u, const boost::property_tree::ptree &cfg)
{
    // stupid mechanical work
    bool is_good_cfg = false;
    try
    {
        u.spinBox_jpeg_quality->setValue(cfg.get<int>("cfg.img.q"));
        u.spinBox_rst_num->setValue(cfg.get<int>("cfg.img.rst"));
        u.checkBox_is_gray->setChecked(cfg.get<bool>("cfg.img.bw"));
        u.spinBox_bch_m->setValue(cfg.get<int>("cfg.bch.n"));
        u.spinBox_bch_t->setValue(cfg.get<int>("cfg.bch.t"));
        u.spinBox_fps_limit->setValue(cfg.get<int>("cfg.fps.lim"));

        u.doubleSpinBox_error_persent->setValue(cfg.get<double>("cfg.error.val"));

        u.spinBox_port_data->setValue(cfg.get<int>("cfg.dataport"));
        u.spinBox_port_cmd->setValue(cfg.get<int>("cfg.cmdport"));
        u.spinBox_bw->setValue(cfg.get<int>("cfg.bw"));

        ui_set_resolution_index(u, cfg);
        ui_set_bch_preset_list_index(u, cfg);

        is_good_cfg = true;
    }
    catch (std::exception& ex)
    {
    }
    return is_good_cfg;
}

#if 0
void update_stat(Ui::MainWindow & u, const std::string& data)
{
    std::stringstream ss(data);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    u.lineEdit_proc_time->setText(
                QString::fromUtf8(pt.get<std::string>("t.proc").c_str())
                );
    u.lineEdit_send_time->setText(
                QString::fromUtf8(pt.get<std::string>("t.send").c_str())
                );

    u.spinBox_cap_fps->setValue(pt.get<unsigned int>("cam.fps"));
    u.spinBox_proc_fps->setValue(pt.get<unsigned int>("proc.fps"));
    u.spinBox_sent_frames->setValue(pt.get<unsigned int>("sent.frames"));

    u.lineEdit_frame_size->setText(
                QString::fromUtf8(pt.get<std::string>("fr.size").c_str())
                );
    u.lineEdit_num_rst->setText(
                QString::fromUtf8(pt.get<std::string>("rst.num").c_str())
                );


    double speed = pt.get<double>("sent.bytes");
    double mbps = speed*8.0/1000000.0;

    u.lineEdit_sent_bytes->setText(QString::number(mbps));

    u.lineEdit_ecc_payload_coef->setText(
                QString::fromUtf8(pt.get<std::string>("ecc.coef").c_str())
                );

    // update jpeg quality if needed
    int jpeg_auto_q = pt.get<int>("jpg.a.q");
    if (u.spinBox_jpeg_quality->value() != jpeg_auto_q)
    {
        u.spinBox_jpeg_quality->setValue(jpeg_auto_q);
    }

}
#endif

void cfg_update(boost::property_tree::ptree &cfg, const Ui::SettingsDialog &u)
{
    cfg.put("cfg.bch.n",     u.spinBox_bch_m->value());
    cfg.put("cfg.bch.t",     u.spinBox_bch_t->value());
    cfg.put("cfg.img.q",     u.spinBox_jpeg_quality->value());
    cfg.put("cfg.dataport",  u.spinBox_port_data->value());
    cfg.put("cfg.img.rst",   u.spinBox_rst_num->value());
    cfg.put("cfg.img.bw",    u.checkBox_is_gray->isChecked());
    cfg.put("cfg.error.val", u.doubleSpinBox_error_persent->value());
    cfg.put("cfg.fps.lim",   u.spinBox_fps_limit->value());

    cfg.put("cfg.cmdport", u.spinBox_port_cmd->value());
    cfg.put("cfg.dataport", u.spinBox_port_data->value());
    cfg.put("cfg.bw", u.spinBox_bw->value());

    cfg_set_resolution_by_list_index(cfg
                                     , u.comboBox_camera_resolution->currentIndex());

}

int ui_set_resolution_index(Ui::SettingsDialog& u, const boost::property_tree::ptree& cfg)
{
    int def_idx = 2; // 640x480 by default

    int w = cfg.get<int>("cfg.img.width");
    int h = cfg.get<int>("cfg.img.height");

    for(int i = 0; i < RES_MODE_MAX; i++)
    {
        if (res_modes[i].one == w && res_modes[i].two == h)
        {
            def_idx = i;
            break;
        }
    }

    u.comboBox_camera_resolution->setCurrentIndex(def_idx);

    return def_idx;
}

int cfg_set_resolution_by_list_index(boost::property_tree::ptree &cfg, int idx)
{
    if (idx >= 0 && idx < RES_MODE_MAX)
    {
        int w = res_modes[idx].one;
        int h = res_modes[idx].two;
        cfg.put("cfg.img.width", w);
        cfg.put("cfg.img.height", h);
    }
    else
    {
        std::cerr << "camera resulution wrong index ! " << idx << std::endl;
        return -1;
    }
    return 0;
}

int ui_set_bch_preset_list_index(Ui::SettingsDialog &u
                                 , const boost::property_tree::ptree &cfg)
{
    int def_idx = BCH_PRESET_MAX; // custom

    int m = cfg.get<int>("cfg.bch.n");
    int t = cfg.get<int>("cfg.bch.t");

    for(size_t i = 0; i < BCH_PRESET_MAX; i++)
    {
        if (bch_modes[i].one == m && bch_modes[i].two == t)
        {
            def_idx = static_cast<int>(i);
            break;
        }
    }
    if (u.comboBox_bch_mode->currentIndex() != def_idx)
    {
        u.comboBox_bch_mode->setCurrentIndex(def_idx);
    }

    return 0;
}

int cfg_set_bch_values_by_list_index(boost::property_tree::ptree& cfg
                                     , Ui::SettingsDialog & u, int idx)
{
    if (idx >= 0 && idx <= BCH_PRESET_MAX)
    {
        int n = u.spinBox_bch_m->value();
        int t = u.spinBox_bch_t->value();
        bool enabled = true;
        if (idx < BCH_PRESET_MAX)
        {
            n = bch_modes[idx].one;
            t = bch_modes[idx].two;
            enabled = false;
        }

        cfg.put("cfg.bch.n", n);
        cfg.put("cfg.bch.t", t);

        // update values
        u.spinBox_bch_m->setValue(n);
        u.spinBox_bch_t->setValue(t);
        u.spinBox_bch_m->setEnabled(enabled);
        u.spinBox_bch_t->setEnabled(enabled);

    }
    else
    {
        std::cerr << "bch preset wrong index ! " << idx << std::endl;
        return -1;
    }
    return 0;
}

bool read_config_file(cfg_ptr_t cfg, const std::string & fname)
{
    bool ret_val = false;
    try
    {
        boost::property_tree::read_json(fname, *cfg);
        ret_val = true;
    }
    catch(...)
    {
    }
    return ret_val;
}

bool write_config_file(cfg_ptr_t cfg, const std::string & fname)
{
    bool ret_val = false;
    try
    {
        boost::property_tree::write_json(fname, *cfg);
        ret_val = true;
    }
    catch(...)
    {
    }
    return ret_val;
}


void init_config_defaults(cfg_ptr_t cfg)
{
    cfg->put("cfg.bch.n",  0);
    cfg->put("cfg.bch.t",  0);
    cfg->put("cfg.img.q",  100);
    cfg->put("cfg.dataport", 9950);
    cfg->put("cfg.img.rst",   1);
    cfg->put("cfg.img.bw",    0);
    cfg->put("cfg.error.val", 0.0f);
    cfg->put("cfg.fps.lim",   25);

    cfg->put("cfg.cmdport", 9900);
    cfg->put("cfg.dataport", 9950);
    cfg->put("cfg.bw", 10);

    cfg_set_resolution_by_list_index(*cfg, 2);
}



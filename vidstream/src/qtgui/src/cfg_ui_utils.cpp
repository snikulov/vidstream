#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    {13,105}
    ,{5,3}
    ,{6,6}
    ,{5,4}
};
const size_t BCH_PRESET_MAX = sizeof(bch_modes)/sizeof(bch_modes[0]);

bool ui_update(Ui::MainWindow &u, const boost::property_tree::ptree &cfg)
{
    // stupid mechanical work
    bool is_good_cfg = false;
    try
    {
        u.spinBox_chrome_quality->setValue(cfg.get<int>("cfg.img.chrom"));
        u.spinBox_lum_quality->setValue(cfg.get<int>("cfg.img.lum"));
        u.spinBox_rst_num->setValue(cfg.get<int>("cfg.img.rst"));
        u.checkBox_is_gray->setChecked(cfg.get<bool>("cfg.img.bw"));
        u.spinBox_bch_m->setValue(cfg.get<int>("cfg.bch.m"));
        u.spinBox_bch_t->setValue(cfg.get<int>("cfg.bch.t"));

        u.doubleSpinBox_error_persent->setValue(cfg.get<double>("cfg.error.val"));

        u.spinBox_port_data->setValue(cfg.get<int>("cfg.dataport"));
        u.spinBox_port_cmd->setValue(cfg.get<int>("cfg.cmdport"));

        ui_set_resolution_index(u, cfg);
        ui_set_bch_preset_list_index(u, cfg);

        is_good_cfg = true;
    }
    catch (std::exception& ex)
    {
    }
    return is_good_cfg;
}

void cfg_update(boost::property_tree::ptree &cfg, const Ui::MainWindow &u)
{
    cfg.put("cfg.bch.m",     u.spinBox_bch_m->value());
    cfg.put("cfg.bch.t",     u.spinBox_bch_t->value());
    cfg.put("cfg.img.chrom", u.spinBox_chrome_quality->value());
    cfg.put("cfg.img.lum",   u.spinBox_lum_quality->value());
    cfg.put("cfg.dataport",  u.spinBox_port_data->value());
    cfg.put("cfg.img.rst",   u.spinBox_rst_num->value());
    cfg.put("cfg.img.bw",    u.checkBox_is_gray->isChecked());
    cfg.put("cfg.error.val", u.doubleSpinBox_error_persent->value());

    cfg.put("cfg.cmdport", u.spinBox_port_cmd->value());
    cfg.put("cfg.dataport", u.spinBox_port_data->value());

    cfg_set_resolution_by_list_index(cfg
                                     , u.comboBox_camera_resolution->currentIndex());

}

int ui_set_resolution_index(Ui::MainWindow& u, const boost::property_tree::ptree& cfg)
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

int ui_set_bch_preset_list_index(Ui::MainWindow &u
                                 , const boost::property_tree::ptree &cfg)
{
    int def_idx = BCH_PRESET_MAX; // custom

    int m = cfg.get<int>("cfg.bch.m");
    int t = cfg.get<int>("cfg.bch.t");

    for(int i = 0; i < BCH_PRESET_MAX; i++)
    {
        if (bch_modes[i].one == m && bch_modes[i].two == t)
        {
            def_idx = i;
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
                                     , Ui::MainWindow& u, int idx)
{
    if (idx >= 0 && idx <= BCH_PRESET_MAX)
    {
        int m = u.spinBox_bch_m->value();
        int t = u.spinBox_bch_t->value();

        if (idx < BCH_PRESET_MAX)
        {
            m = bch_modes[idx].one;
            t = bch_modes[idx].two;
            if (u.spinBox_bch_m->value() != m) u.spinBox_bch_m->setValue(m);
            if (u.spinBox_bch_t->value() != t) u.spinBox_bch_t->setValue(t);
        }
        cfg.put("cfg.bch.m", m);
        cfg.put("cfg.bch.t", t);
    }
    else
    {
        std::cerr << "bch preset wrong index ! " << idx << std::endl;
        return -1;
    }
    return 0;
}

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <boost/foreach.hpp>

#include <iostream>
#include <service_worker.hpp>



void display(const int depth, const boost::property_tree::ptree& tree)
{
    using namespace boost::property_tree;
    using namespace std;

    BOOST_FOREACH( ptree::value_type const&v, tree.get_child("") ) {
        ptree subtree = v.second;
        string nodestr = tree.get<string>(v.first);

        // print current node
        cout << string("").assign(depth*2,' ') << "* ";
        cout << v.first;
        if ( nodestr.length() > 0 )
            cout << "=\"" << tree.get<string>(v.first) << "\"";
        cout << endl;

        // recursive go down the hierarchy
        display(depth+1,subtree);
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    is_srv_running_(false)
    , cfg_(new boost::property_tree::ptree())
{
    bool file_exist = false;
    try
    {
        boost::property_tree::read_json("settings.json", *cfg_);
        display(3, *cfg_);
        file_exist = true;
    }
    catch(std::exception ex)
    {
    }

    ui->setupUi(this);

    if (file_exist)
    {
        // TODO: reload values from file
        if(!update_ui(ui, *cfg_))
        {
            update_cfg(*cfg_, ui);
        }
    }
    else
    {
        update_cfg(*cfg_, ui);
    }

    // pass configuration to logic
    logic_.reset(new service_worker(cfg_));

}

bool MainWindow::update_ui(Ui::MainWindow *gui, const boost::property_tree::ptree& cfg)
{
    // stupid mechanical work
    bool is_good_cfg = false;
    try
    {
        gui->spinBox_bch_m->setValue(cfg.get<int>("cfg.bch.m"));
        gui->spinBox_bch_t->setValue(cfg.get<int>("cfg.bch.t"));
        gui->spinBox_chrome_quality->setValue(cfg.get<int>("cfg.img.chrom"));
        gui->spinBox_lum_quality->setValue(cfg.get<int>("cfg.img.lum"));
        gui->spinBox_port_data->setValue(cfg.get<int>("cfg.dataport"));
        gui->spinBox_rst_num->setValue(cfg.get<int>("cfg.img.rst"));
        gui->checkBox_is_gray->setEnabled(cfg.get<bool>("cfg.img.bw"));
        is_good_cfg = true;
    }
    catch (std::exception ex)
    {
    }
    return is_good_cfg;
}

void MainWindow::update_cfg(boost::property_tree::ptree& cfg, const Ui::MainWindow *gui)
{
    cfg.put("cfg.bch.m", gui->spinBox_bch_m->value());
    cfg.put("cfg.bch.t", gui->spinBox_bch_t->value());
    cfg.put("cfg.img.chrom", gui->spinBox_chrome_quality->value());
    cfg.put("cfg.img.lum", gui->spinBox_lum_quality->value());
    cfg.put("cfg.dataport", gui->spinBox_port_data->value());
    cfg.put("cfg.img.rst", gui->spinBox_rst_num->value());
    cfg.put("cfg.img.bw", gui->checkBox_is_gray->isChecked());
}

MainWindow::~MainWindow()
{
    boost::property_tree::write_json("settings.json", *cfg_);
    delete ui;
}

void MainWindow::on_checkBox_is_gray_stateChanged(int arg1)
{
    // grayscale toggled
    bool val = arg1 ? true : false;
    cfg_->put("cfg.img.bw", val);
}

void MainWindow::on_comboBox_camera_resolution_currentIndexChanged(int index)
{
    // current resolution changed
    typedef struct
    {
        int width;
        int height;
    } scr_size_t;

    static const scr_size_t modes[] = {{1280,800},{1024,780},{640,480},{320,240},{0, 0}};
    size_t max_size = sizeof(modes)/sizeof(modes[0]);

    if (index >= 0 && index < max_size)
    {
        int w = modes[index].width;
        int h = modes[index].height;
        cfg_->put("cfg.img.width", w);
        cfg_->put("cfg.img.height", h);
    }
    else
    {
        std::cerr << "camera resulution wrong index ! " << index << std::endl;
    }
}

void MainWindow::on_pushButton_operate_clicked()
{
    // run the program

    // toggle flag
    is_srv_running_ = !is_srv_running_;
    if (is_srv_running_)
    {
        ui->pushButton_operate->setText("Стоп");
        logic_->start();
    }
    else
    {
        ui->pushButton_operate->setText("Запуск");
        logic_->stop();
    }
    // disable changes for prots
    ui->spinBox_port_cmd->setDisabled(is_srv_running_);
    ui->spinBox_port_data->setDisabled(is_srv_running_);
}

void MainWindow::on_spinBox_rst_num_valueChanged(int arg1)
{
    // rst value
    cfg_->put("cfg.img.rst", arg1);
}

void MainWindow::on_spinBox_port_cmd_valueChanged(int arg1)
{
    // control port
    cfg_->put("cfg.cmdport", arg1);
}

void MainWindow::on_spinBox_port_data_valueChanged(int arg1)
{
    // data port
    cfg_->put("cfg.dataport", arg1);
}

void MainWindow::on_lineEdit_channel_speed_textChanged(const QString &arg1)
{
    // channel speed changed
    std::cerr << "line speed changed" << std::endl;
}

void MainWindow::on_comboBox_bch_mode_currentIndexChanged(int index)
{
    // bch preset changed
    std::cerr << "bch prameters changed: " << index << std::endl;
}

void MainWindow::on_spinBox_bch_m_valueChanged(int arg1)
{
    // bch m value
    cfg_->put("cfg.bch.m", arg1);
}

void MainWindow::on_spinBox_bch_t_valueChanged(int arg1)
{
    // bch t value
    cfg_->put("cfg.bch.t", arg1);
}

void MainWindow::on_spinBox_error_persent_valueChanged(int arg1)
{
    // line error changed
    std::cerr << "line error " << arg1 << "%" << std::endl;
}

void MainWindow::on_spinBox_lum_quality_valueChanged(int arg1)
{
    // lum value changed
    cfg_->put("cfg.img.lum", arg1);
}

void MainWindow::on_spinBox_chrome_quality_valueChanged(int arg1)
{
    // chrome
    cfg_->put("cfg.img.chrom", arg1);
}

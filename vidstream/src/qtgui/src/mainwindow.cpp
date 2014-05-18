#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <service_worker.hpp>

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
        file_exist = true;
    }
    catch(std::exception ex)
    {
    }

    ui->setupUi(this);

    if (file_exist)
    {
        // TODO: reload values from file
    }

    // pass configuration to logic
    logic_.reset(new service_worker(cfg_));

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

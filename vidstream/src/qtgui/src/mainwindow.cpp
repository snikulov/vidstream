#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    is_srv_running_(false)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_checkBox_is_gray_stateChanged(int arg1)
{
    // grayscale toggled
    std::cerr << "grayscale changed: " << arg1 << std::endl;
}

void MainWindow::on_comboBox_camera_resolution_currentIndexChanged(int index)
{
    // current resolution changed
    std::cerr << "camera resulutio index changed: " << index << std::endl;
}

void MainWindow::on_pushButton_operate_clicked()
{
    // run the program

    // toggle flag
    is_srv_running_ = is_srv_running_ ? !is_srv_running_ : !is_srv_running_;
    if (is_srv_running_)
    {
        ui->pushButton_operate->setText("Стоп");
    }
    else
    {
        ui->pushButton_operate->setText("Запуск");
    }
    ui->spinBox_port_cmd->setDisabled(is_srv_running_);
    ui->spinBox_port_data->setDisabled(is_srv_running_);
}

void MainWindow::on_spinBox_rst_num_valueChanged(int arg1)
{
    // rst value
    std::cerr << "rst value changed: " << arg1 << std::endl;
}

void MainWindow::on_spinBox_port_cmd_valueChanged(int arg1)
{
    // control port
    std::cerr << "control port value changed: " << arg1 << std::endl;
}

void MainWindow::on_spinBox_port_data_valueChanged(int arg1)
{
    // data port
    std::cerr << "data port value changed: " << arg1 << std::endl;
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
    std::cerr << "bch m = " << arg1 << std::endl;
}

void MainWindow::on_spinBox_bch_t_valueChanged(int arg1)
{
    // bch t value
    std::cerr << "bch t = " << arg1 << std::endl;
}

void MainWindow::on_spinBox_error_persent_valueChanged(int arg1)
{
    // line error changed
    std::cerr << "line error " << arg1 << "%" << std::endl;
}

void MainWindow::on_spinBox_lum_quality_valueChanged(int arg1)
{
    // lum value changed
    std::cerr << "lum value = " << arg1 << std::endl;
}

void MainWindow::on_spinBox_chrome_quality_valueChanged(int arg1)
{
    // chrome
    std::cerr << "lum value = " << arg1 << std::endl;
}

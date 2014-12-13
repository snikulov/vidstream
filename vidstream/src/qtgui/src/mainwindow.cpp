#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <boost/foreach.hpp>

#include <iostream>
#include <service_worker.hpp>

#include <QNetworkInterface>
#include <QHostAddress>


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

    if (file_exist)
    {
        // TODO: reload values from file
        if(!ui_update(*ui, *cfg_))
        {
            cfg_update(*cfg_, *ui);
        }
    }
    else
    {
        cfg_update(*cfg_, *ui);
    }

    // pass configuration to logic
    logic_.reset(new service_worker(*ui, cfg_));

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
    cfg_set_resolution_by_list_index(*cfg_, index);
}

void MainWindow::on_pushButton_operate_clicked()
{
    // run the program

    // toggle flag
    is_srv_running_ = !is_srv_running_;
    if (is_srv_running_)
    {
        ui->pushButton_operate->setText("Stop");
        logic_->start();
    }
    else
    {
        ui->pushButton_operate->setText("Start");
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
//    std::cerr << "bch prameters changed: " << index << std::endl;
    cfg_set_bch_values_by_list_index(*cfg_, *ui, index);

}

void MainWindow::on_spinBox_bch_m_valueChanged(int arg1)
{
    // bch m value

    ui_set_bch_preset_list_index(*ui, *cfg_);
    if (0 == arg1 || (arg1 >= 5 && arg1 <= 15))
    {
        cfg_->put("cfg.bch.n", arg1);
    }
}

void MainWindow::on_spinBox_bch_t_valueChanged(int arg1)
{
    // bch t value
    cfg_->put("cfg.bch.t", arg1);
    ui_set_bch_preset_list_index(*ui, *cfg_);
}

void MainWindow::on_doubleSpinBox_error_persent_valueChanged(double arg1)
{
    // changed error value
    cfg_->put("cfg.error.val", arg1);
}

void MainWindow::on_spinBox_bch_m_editingFinished()
{

}

void MainWindow::on_lineEdit_sent_bytes_textChanged(const QString &arg1)
{
    double mbps = ui->lineEdit_sent_bytes->text().toDouble();
    if (mbps > ui->spinBox_bw->value())
    {
        ui->lineEdit_sent_bytes->setStyleSheet("QLineEdit{background: red;}");
    }
    else
    {
        ui->lineEdit_sent_bytes->setStyleSheet("QLineEdit{background: white;}");
    }
}

void MainWindow::on_spinBox_bw_valueChanged(int arg1)
{
    cfg_->put("cfg.bw", arg1);
}

void MainWindow::on_spinBox_jpeg_quality_valueChanged(int arg1)
{
    cfg_->put("cfg.img.q", arg1);
}

void MainWindow::on_spinBox_fps_limit_valueChanged(int arg1)
{
    cfg_->put("cfg.fps.lim", arg1);
}

void MainWindow::on_comboBox_ip_selector_currentIndexChanged(int index)
{
    cfg_->put("cfg.ip",
              ui->comboBox_ip_selector->itemText(index).toUtf8().constData());
}

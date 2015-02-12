#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settingsdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    , cfg_(new boost::property_tree::ptree)
{
    ui->setupUi(this);
    if(!read_config_file("settings.json", cfg_))
    {
        init_config_defaults(cfg_);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionStart_triggered()
{
    // start trigged
    // run capture
}

void MainWindow::on_actionConfigure_triggered()
{
    // Settings trigged
    SettingsDialog dlg(cfg_);
    dlg.exec();

}

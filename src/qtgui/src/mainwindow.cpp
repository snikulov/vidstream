#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settingsdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    SettingsDialog dlg;
    dlg.exec();

}

#include "mainwindow.h"
#include "ui_mainwindow.h"

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

void MainWindow::on_pushButton_clicked()
{
    // clicked Start button
}

void MainWindow::on_checkBox_is_gray_stateChanged(int arg1)
{
    // grayscale toggled
}

void MainWindow::on_comboBox_camera_resolution_currentIndexChanged(int index)
{
    // current resolution changed
}

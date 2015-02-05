#include "mainvideoiowindow.h"
#include "ui_mainvideoiowindow.h"

MainVideoIoWindow::MainVideoIoWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainVideoIoWindow)
{
    ui->setupUi(this);
}

MainVideoIoWindow::~MainVideoIoWindow()
{
    delete ui;
}

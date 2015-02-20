#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settingsdialog.h"
#include "service_worker.hpp"

static QImage mat_to_qimg(cv::Mat const& src)
{
     cv::Mat temp; // make the same cv::Mat
     cvtColor(src, temp,cv::COLOR_BGR2RGB); // cvtColor Makes a copt, that what i need
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy, see documentation
     // of QImage::QImage ( const uchar * data, int width, int height, Format format )
     return dest;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    , cfg_(new boost::property_tree::ptree)
    , do_capture_(false)
{
    ui->setupUi(this);
    if(!read_config_file(cfg_))
    {
        init_config_defaults(cfg_);
    }

    connect(this, SIGNAL(signal_show_image(QImage)),
            SLOT(slot_show_image(QImage)),
            Qt::QueuedConnection);

    worker_.reset(new service_worker(*this, cfg_));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionStart_triggered()
{
    // start trigged
    // run capture
    if (!do_capture_)
    {
        do_capture_ = true;
        worker_->start();
    }
    else
    {
        do_capture_ = false;
        worker_->stop();
    }
}

void MainWindow::on_actionConfigure_triggered()
{
    // Settings trigged
    SettingsDialog dlg(cfg_);
    dlg.exec();

}

void MainWindow::slot_show_image(QImage img)
{
    std::cerr << "slot show_image triggered" << std::endl;
}

void MainWindow::post_image(cv::Mat * img)
{
    QImage qi = mat_to_qimg(*img);
    emit signal_show_image(qi);
}

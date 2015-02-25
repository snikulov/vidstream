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
    , scene_(new QGraphicsScene)
    , item_(new QGraphicsPixmapItem)
    , cfg_(new boost::property_tree::ptree)
    , do_capture_(false)
{
    ui->setupUi(this);
    if(!read_config_file(cfg_))
    {
        init_config_defaults(cfg_);
    }

    adjust_size();

    ui->graphicsView->setScene(scene_);

    connect(this, SIGNAL(signal_show_image(QImage *)),
            SLOT(slot_show_image(QImage *)),
            Qt::QueuedConnection);

    worker_.reset(new service_worker(*this, cfg_));
}

MainWindow::~MainWindow()
{
    delete item_;
    delete scene_;
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

    adjust_size();

}

void MainWindow::slot_show_image(QImage * img)
{
    std::cerr << "slot show_image triggered" << std::endl;

    item_->setPixmap(QPixmap::fromImage(*img));

    scene_->addItem(item_);

    ui->graphicsView->show();

    delete img;

}

void MainWindow::post_image(mat_ptr_t img)
{
    QImage * qi = new QImage( mat_to_qimg(*img) );
    emit signal_show_image(qi);
}

void MainWindow::adjust_size()
{
    const int diff = 20;
    int w = cfg_->get<int>("cfg.img.width");
    int h = cfg_->get<int>("cfg.img.height");

    resize(w+diff, h+(diff*4));
    ui->graphicsView->resize(w, h);

//    ui->centralWidget->setBaseSize(w, h);
}

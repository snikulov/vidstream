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

//    adjust_size();

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

    showMaximized();
    ui->graphicsView->move(ui->centralWidget->geometry().center());
    ui->graphicsView->resize(w, h);

//    ui->centralWidget->setBaseSize(w, h);
}

void MainWindow::update_stat(const std::string& data)
{
    Ui::MainWindow& u = *ui;

    std::stringstream ss(data);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    // capture fps...
    u.label_0->setText(QString("cFPS: ")
            + QString::fromUtf8(pt.get<std::string>("cam.fps").c_str())
            );
    // processing fps...
    u.label_1->setText(QString("pFPS: ")
            + QString::fromUtf8(pt.get<std::string>("proc.fps").c_str())
            );
    // frames sent...
    u.label_2->setText(QString("Fsent: ")
            + QString::fromUtf8(pt.get<std::string>("sent.frames").c_str())
            );

    // frame size...
    u.label_3->setText(QString("Fsize: ")
                + QString::fromUtf8(pt.get<std::string>("fr.size").c_str())
                );
    // num rst in frame..
    u.label_4->setText(QString("Nrst: ")
                + QString::fromUtf8(pt.get<std::string>("rst.num").c_str())
                );
    // required bw...
    double speed = pt.get<double>("sent.bytes");
    double mbps = speed*8.0/1000000.0;
    u.label_5->setText(QString("BW: ")
            + QString::number(mbps)
            );
    u.label_6->setText(QString("ECCoh: ")
                + QString::fromUtf8(pt.get<std::string>("ecc.coef").c_str())
                );

#if 0
    u.lineEdit_send_time->setText(
                QString::fromUtf8(pt.get<std::string>("t.send").c_str())
                );


#endif
    // update jpeg quality if needed
    int jpeg_auto_q = pt.get<int>("jpg.a.q");
    int jpeg_cfg_q = cfg_->get<int>("cfg.img.q");

    if (jpeg_cfg_q != jpeg_auto_q)
    {
        cfg_->put("cfg.img.q", jpeg_auto_q);
    }

}



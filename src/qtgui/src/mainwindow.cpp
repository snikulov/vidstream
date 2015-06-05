#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settingsdialog.h"
#include "service_worker.hpp"
#include "config_iface.hpp"



static QImage mat_to_qimg(cv::Mat const& src)
{
     cv::Mat temp; // make the same cv::Mat
     cvtColor(src, temp,cv::COLOR_BGR2RGB); // cvtColor Makes a copt, that what i need
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy, see documentation
     // of QImage::QImage ( const uchar * data, int width, int height, Format format )
     return dest;
}


MainWindow::MainWindow(bool cfg, QWidget *parent) :
    QMainWindow(parent)
    , is_cfg_enabled_(cfg)
    , ui(new Ui::MainWindow)
    , scene_(new QGraphicsScene)
    , item_(new QGraphicsPixmapItem)
    , cfg_(new boost::property_tree::ptree)
    , do_capture_(false)
    , resolutions_(get_resolution_list())
    , bchmodes_(get_bch_mode_list())
{
    ui->setupUi(this);
    if(!read_config_file(cfg_, CONFIG_FILE_NAME))
    {
        init_config_defaults(cfg_);
        write_config_file(cfg_, CONFIG_FILE_NAME);
    }

    cfg_ptr_t pset(new boost::property_tree::ptree);
    if(read_config_file(pset, PROPERTIES_FILE_NAME))
    {
        presets_ = get_profile_list(pset);

        if (presets_)
        {
            for(size_t i = 0; i < presets_->size(); ++i)
            {
                QString pname = QString::fromUtf8((*presets_)[i].name_.c_str());
                ui->comboBox_profiles->addItem(pname);
            }
        }
    }

    adjust_size();
    adjust_preset();

    ui->graphicsView->setScene(scene_);

    if (!is_cfg_enabled_)
    {
        ui->menuSettings->setEnabled(is_cfg_enabled_);
        ui->menuSettings->setTitle("");
        ui->menuBar->removeAction(ui->actionConfigure);
    }

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

void MainWindow::adjust_preset()
{
    profile p(""
              , cfg_->get<int>("cfg.bw")
              , cfg_->get<int>("cfg.fps.lim")
              , cfg_->get<int>("cfg.img.q")
              , find_index(bchmodes_, std::make_pair(cfg_->get<int>("cfg.bch.n"), cfg_->get<int>("cfg.bch.t")))
              , find_index(resolutions_, std::make_pair(cfg_->get<int>("cfg.img.width"), cfg_->get<int>("cfg.img.height")))
              );

    int idx = find_index(presets_, p);
    idx++;
    ui->comboBox_profiles->setCurrentIndex(idx);
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
    SettingsDialog dlg(cfg_, presets_);
    dlg.exec();

    // update profile index
    adjust_preset();
}

void MainWindow::slot_show_image(QImage * img)
{
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
    int w = cfg_->get<int>("cfg.img.width");
    int h = cfg_->get<int>("cfg.img.height");

    showMaximized();
    ui->graphicsView->move(ui->centralWidget->geometry().center());
    ui->graphicsView->resize(w, h);
}

void MainWindow::update_stat(const std::string& data)
{
    Ui::MainWindow& u = *ui;

    std::stringstream ss(data);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    // capture fps...
    u.label_0->setText(QString("Capture FPS: ")
            + QString::fromUtf8(pt.get<std::string>("cam.fps").c_str())
            );
    // processing fps...
    u.label_1->setText(QString("Processing FPS: ")
            + QString::fromUtf8(pt.get<std::string>("proc.fps").c_str())
            );
    // frames sent...
    u.label_2->setText(QString("Frames Sent: ")
            + QString::fromUtf8(pt.get<std::string>("sent.frames").c_str())
            );

    // frame size...
    u.label_3->setText(QString("Frame size: ")
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
    u.label_6->setText(QString("ECC increase: ")
                + QString::fromUtf8(pt.get<std::string>("ecc.coef").c_str())
                );

    // update jpeg quality if needed
    int jpeg_auto_q = pt.get<int>("jpg.a.q");
    int jpeg_cfg_q = cfg_->get<int>("cfg.img.q");

    u.label_7->setText(QString("Quality: ")
                       + QString::number(jpeg_auto_q)
                       );

    if (jpeg_cfg_q != jpeg_auto_q)
    {
        cfg_->put("cfg.img.q", jpeg_auto_q);
    }

}

void MainWindow::on_comboBox_profiles_currentIndexChanged(int index)
{
    // TODO: remove copy-paste from settingsdialog.ui

    // changed profile index
    if (presets_)
    {
        const profile_list_t& presets = *presets_;

        size_t i = static_cast<size_t>(index);

        if (i > 0 && i <= presets.size())
        {
            // correcting i to point profile in vector
            i--;
            const profile & rp = presets[i];

            cfg_->put("cfg.bw", rp.bw_);
            cfg_->put("cfg.fps.lim", rp.fps_);
            cfg_->put("cfg.img.q", rp.jpeg_quality_);

            std::pair<int, int> p = resolutions_->at(rp.res_idx_);

            cfg_->put("cfg.img.width", p.first);
            cfg_->put("cfg.img.height", p.second);

            p = bchmodes_->at(rp.bch_idx_);
            cfg_->put("cfg.bch.n", p.first);
            cfg_->put("cfg.bch.t", p.second);
        }
        else
        {
            // defaults
            cfg_->put("cfg.bch.n",  0);
            cfg_->put("cfg.bch.t",  0);
            cfg_->put("cfg.img.q",  100);
            cfg_->put("cfg.error.val", 0.0f);
            cfg_->put("cfg.fps.lim",   25);
            cfg_->put("cfg.bw", 10);
        }
        write_config_file(cfg_, CONFIG_FILE_NAME);
    }
}

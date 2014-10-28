#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <service_worker.hpp>
#include <ocv/ocv_output.hpp>

#include <jpeg_receiver.hpp>
#include <ctrlsrv.hpp>

service_worker::service_worker(Ui::MainWindow &u, boost::shared_ptr<boost::property_tree::ptree> pcfg)
    : ui_(u), cfg_(pcfg), stop_(false)
{
}

service_worker::~service_worker()
{
}

void service_worker::start()
{
    stop_ = false;
    // init run

    //cv::namedWindow("received");

    std::string host("tcp://127.0.0.1:");
    std::string dataurl = host + cfg_->get<std::string>("cfg.dataport");
    std::string cmdurl = host + cfg_->get<std::string>("cfg.cmdport");

    cfgsrv_.reset(new ctrlsrv(ui_, cfg_, cmdurl, stop_));
    boost::shared_ptr<jpeg_builder> jb(new jpeg_builder());
    boost::shared_ptr<corrupt_intro> err(new corrupt_intro());

    cfgsrv_->subscribe(jb.get());
    cfgsrv_->subscribe(err.get());


    int m = cfg_->get<int>("cfg.bch.m");
    int t = cfg_->get<int>("cfg.bch.t");
//    bch_.reset(new bch_codec(m, t)); // bm, bt
//    cfgsrv_->subscribe(bch_.get());
    rcv_.reset(new jpeg_receiver(stop_, dataurl, err, jb));

    // run threads
    cfgthread_.reset(new boost::thread(*cfgsrv_));
    process_.reset(new boost::thread(*rcv_));

}

void service_worker::stop()
{
    // terminate all work
    stop_ = true;

    rcv_->stop();

    process_->join();
    cfgthread_->join();
    cv::destroyWindow("received");
}

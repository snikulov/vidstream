#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <service_worker.hpp>
#include <ocv/ocv_output.hpp>

#include <jpeg_receiver.hpp>
#include <ctrlsrv.hpp>

#include <boost/function.hpp>

service_worker::service_worker(MainWindow &u, boost::shared_ptr<boost::property_tree::ptree> pcfg)
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
    boost::function<void(mat_ptr_t)> pfunc =
        std::bind1st(std::mem_fun(&MainWindow::post_image), &ui_);

    std::string proto("tcp://");
    std::string host = cfg_->get<std::string>("cfg.ip");
    std::string dataurl = proto + host + ":"
            + cfg_->get<std::string>("cfg.dataport");
    std::string cmdurl = proto + host + ":"
            + cfg_->get<std::string>("cfg.cmdport");

    cfgsrv_.reset(new ctrlsrv(ui_, cfg_, cmdurl, stop_));
    boost::shared_ptr<jpeg_builder> jb(new jpeg_builder());
    boost::shared_ptr<corrupt_intro> err(new corrupt_intro());


    int m = cfg_->get<int>("cfg.bch.n");
    int t = cfg_->get<int>("cfg.bch.t");
    boost::shared_ptr<bchwrapper> bch(new bchwrapper(m, t));
    rcv_.reset(new jpeg_receiver(stop_, dataurl, err, jb, bch, pfunc));
    cfgsrv_->subscribe(rcv_.get());

    // run threads
    cfgthread_.reset(new boost::thread(boost::ref(*cfgsrv_)));
    process_.reset(new boost::thread(*rcv_));

}

void service_worker::stop()
{
    // terminate all work
    stop_ = true;

    rcv_->stop();

    process_->join();
    cfgthread_->join();
}

#include <service_worker.hpp>
#include <ocv/ocv_output.hpp>

#if defined(BUILD_FOR_LINUX)
#include <ecc/ecc.h>
#endif

#include <transport/receiver.hpp>
#include <ctrlsrv.hpp>

service_worker::service_worker(boost::shared_ptr<boost::property_tree::ptree> pcfg)
    : cfg_(pcfg), stop_(false)
{
}

service_worker::~service_worker()
{
}

void service_worker::start()
{
    stop_ = false;
    // init run
    win_.reset(new ocv_output(stop_, "received", mq_));

    std::string host("tcp://127.0.0.1:");
    std::string dataurl = host + cfg_->get<std::string>("cfg.dataport");
    std::string cmdurl = host + cfg_->get<std::string>("cfg.cmdport");

    cfgsrv_.reset(new ctrlsrv(cfg_, cmdurl, stop_));
    boost::shared_ptr<jpeg_builder> jb(new jpeg_builder());
    cfgsrv_->subscribe(jb.get());

#if defined(BUILD_FOR_LINUX)
    int m = cfg_->get<int>("cfg.bch.m");
    int t = cfg_->get<int>("cfg.bch.t");
    bch_.reset(new ecc(m, t)); // bm, bt
    cfgsrv_->subscribe(bch_.get());
    rcv_.reset(new receiver(stop_, dataurl, win_, jb, bch_));

#else
    rcv_.reset(new receiver(stop_, dataurl, win_, jb));
#endif

    // run threads
    cfgthread_.reset(new boost::thread(*cfgsrv_));
    output_.reset(new boost::thread(*win_));
    process_.reset(new boost::thread(*rcv_));

}

void service_worker::stop()
{
    // terminate all work
    stop_ = true;

    win_->stop();
    rcv_->stop();

    cfgthread_->join();
    process_->join();
    output_->join();
}

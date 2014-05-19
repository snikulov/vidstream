#include <service_worker.hpp>
#include <ocv/ocv_output.hpp>

#if defined(BUILD_FOR_LINUX)
#include <ecc/ecc.h>
#endif

#include <transport/receiver.hpp>

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

    std::string url("tcp://127.0.0.1:");
    url += cfg_->get<std::string>("cfg.dataport");

#if defined(BUILD_FOR_LINUX)
    int m = cfg_->get<int>("cfg.bch.m");
    int t = cfg_->get<int>("cfg.bch.t");
    bch_.reset(new ecc(m, t)); // bm, bt
    rcv_.reset(new receiver(stop_, url, win_, bch_));
#else
    rcv_.reset(new receiver(stop_, url, win_));
#endif

    // run threads
    output_.reset(new boost::thread(*win_));
    process_.reset(new boost::thread(*rcv_));

}

void service_worker::stop()
{
    // terminate all work
    stop_ = true;
    win_->stop();
    rcv_->stop();
    process_->join();
    output_->join();
}

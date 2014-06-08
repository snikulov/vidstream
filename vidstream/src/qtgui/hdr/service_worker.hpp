#ifndef SERVICE_WORKER_HPP__
#define SERVICE_WORKER_HPP__

#include <worker.hpp>
#include <monitor_queue.hpp>

#include <opencv2/opencv.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

class ocv_output;
class bch_codec;
class jpeg_receiver;
class ctrlsrv;

class service_worker : public worker, boost::noncopyable
{
public:
    service_worker (boost::shared_ptr<boost::property_tree::ptree>);
    ~service_worker ();

    void start();

    void stop();

private:
    /* data */
    boost::shared_ptr<boost::property_tree::ptree> cfg_;
    boost::shared_ptr<bch_codec> bch_;
    boost::shared_ptr<jpeg_receiver> rcv_;
    boost::shared_ptr<ctrlsrv> cfgsrv_;


    boost::shared_ptr<boost::thread> cfgthread_;
    boost::shared_ptr<boost::thread> process_;

    bool stop_;
};

#endif // SERVICE_WORKER_HPP__

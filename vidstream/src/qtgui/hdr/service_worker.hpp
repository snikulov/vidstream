#ifndef SERVICE_WORKER_HPP__
#define SERVICE_WORKER_HPP__

#include <worker.hpp>
#include <monitor_queue.hpp>


#include <opencv2/opencv.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

class ocv_output;
class ecc;
class receiver;

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
    monitor_queue<cv::Mat> mq_;
    boost::shared_ptr<ocv_output> win_;
    boost::shared_ptr<ecc> bch_;
    boost::shared_ptr<receiver> rcv_;

    boost::shared_ptr<boost::thread> output_;
    boost::shared_ptr<boost::thread> process_;
    bool stop_;
};

#endif // SERVICE_WORKER_HPP__

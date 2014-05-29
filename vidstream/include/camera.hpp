#ifndef CAMERA_HPP__
#define CAMERA_HPP__

#include "frame.hpp"
#include "cfg/cfg_notify.hpp"

#include <string>
#include <exception>

#include <boost/chrono.hpp>
#include <opencv2/opencv.hpp>

using cv::VideoCapture;

namespace vidstream
{

class camera
{
public:
    camera(int w = 640, int h = 480)
        : src_(new VideoCapture()), fname_(), is_hw_cam_(true)
          , req_size_(cv::Size(w,h)), count_(0)
    {
        open();
    }

    camera(const std::string& fname, int w = 640, int h = 480) :
        src_(new VideoCapture()), fname_(fname), is_hw_cam_(false),
        req_size_(cv::Size(w,h))
    {
        open();
    }

    ~camera()
    {
    }

    camera_frame_t get_frame() const
    {
        static int err_count = 0;
        boost::chrono::high_resolution_clock::time_point start = boost::chrono::high_resolution_clock::now();
        camera_frame_t ret_val(new cv::Mat());
        if (src_->read(*ret_val))
        {
            count_++;
        }
        else
        {
            err_count++;
            ret_val.reset();
            if (err_count > 5)
            {
                // re-open camera
                open();
                err_count = 0;
            }
        }
        boost::chrono::high_resolution_clock::time_point end = boost::chrono::high_resolution_clock::now();
        boost::chrono::nanoseconds ndrift = end - start;
        ns_ += ndrift.count();
        unsigned long s = boost::chrono::round<boost::chrono::seconds>(boost::chrono::nanoseconds(ns_)).count();
        if (s) fps_ = count_/s;
        double cvFPS = src_->get(CV_CAP_PROP_FPS);
        std::cout << "Frame count: " << count_ << " FPS: " << fps_ << " cvFPS: " << cvFPS << std::endl;

        return ret_val;
    }



private:
    // init camera
    void open() const
    {
        if(is_hw_cam_)
        {
            src_->open(0);
        }
        else
        {
            src_->open(fname_);
        }

        if (!src_->isOpened())
        {
            std::string source = is_hw_cam_ ? "HW Camera" : fname_;
            throw std::runtime_error(source +": Unable to open");
        }

// TODO: should reconsider to set hw capture prop instead of resize
//        width_  = src_->get(CV_CAP_PROP_FRAME_WIDTH);
//        height_ = src_->get(CV_CAP_PROP_FRAME_HEIGHT);
    }
    /* data */
    boost::scoped_ptr<VideoCapture> src_;
    std::string fname_;
    bool is_hw_cam_;
    cv::Size req_size_;
    mutable unsigned long count_;
    mutable unsigned long ns_;
    mutable double fps_;
};

} /* namespace vidstream */

#endif // CAMERA_HPP__

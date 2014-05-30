#ifndef CAMERA_HPP__
#define CAMERA_HPP__

#include "frame.hpp"
#include "cfg/cfg_notify.hpp"

#include <string>
#include <exception>

#include <boost/chrono.hpp>
#include <opencv2/opencv.hpp>

#include <perf/perf_clock.hpp>

using cv::VideoCapture;

namespace vidstream
{

class camera
{
public:
    camera(int w = 640, int h = 480)
        : src_(new VideoCapture()), fname_(), is_hw_cam_(true)
          , req_size_(cv::Size(w,h)), count_(0), sec_(0)
    {
        open();
    }

    camera(const std::string& fname, int w = 640, int h = 480) :
        src_(new VideoCapture()), fname_(fname), is_hw_cam_(false),
        req_size_(cv::Size(w,h)), count_(0), sec_(0)
    {
        open();
    }

    ~camera()
    {
    }

    camera_frame_t get_frame() const
    {
        timer<steady_clock> t;
        static int err_count = 0;
        camera_frame_t ret_val(new cv::Mat());
        if (src_->read(*ret_val))
        {
            count_++;
            sec_ += t.seconds(); // only good attempts
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

        std::cout << "camera FPS: " << get_soft_fps() << " ocvFPS: " << get_ocv_fps() << std::endl;
        return ret_val;
    }

    double get_soft_fps() const
    {
        return  count_/sec_;
    }
    double get_ocv_fps() const
    {
        return src_->get(CV_CAP_PROP_FPS);
    }

private:
    // init camera
    void open() const
    {
        count_ = 0;
        sec_ = 0.;
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
    mutable double sec_;
};

} /* namespace vidstream */

#endif // CAMERA_HPP__

#ifndef CAMERA_HPP__
#define CAMERA_HPP__

#include "frame.hpp"
#include "cfg/cfg_notify.hpp"

#include <string>
#include <exception>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

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
        , req_size_(cv::Size(w,h)), count_(0), sec_(0),
        read_time_(boost::chrono::high_resolution_clock::now())
    {
        open();
    }

    camera(const std::string& fname, int w = 640, int h = 480) :
        src_(new VideoCapture()), fname_(fname), is_hw_cam_(false),
        req_size_(cv::Size(w,h)), count_(0), sec_(0),
        read_time_(boost::chrono::high_resolution_clock::now())
    {
	open();
    }

    ~camera()
    {
    }

    camera_frame_t get_frame() const
    {
        static int err_count = 0;
        if (!count_)
        {
            timer_.start();
        }
        camera_frame_t ret_val(new cv::Mat());

	// Delay to get desired FPS for fast systems
	double desired_fps = 25.0;


        if (src_->read(*ret_val))
        {
	  // limit read frame rate to 25 fps
	  while ( duration_cast<duration<double> >(high_resolution_clock::now() - read_time_).count() <= 1.0/desired_fps )
	      boost::this_thread::sleep(boost::posix_time::milliseconds(1));

	    read_time_ = boost::chrono::high_resolution_clock::now();
	    count_++;
            timer_.stop();
            sec_ = timer_.seconds(); // only good attempts
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

//        std::cout << "camera FPS: " << get_soft_fps() << " ocvFPS: " << get_ocv_fps() << std::endl;
        return ret_val;
    }

    unsigned int get_soft_fps() const
    {
        return  static_cast<unsigned int>(count_/sec_);
    }
    double get_ocv_fps() const
    {
        return src_->get(cv::CAP_PROP_FPS);
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
	    // limit capture FPS from file
	    // src_->set(CV_CAP_PROP_FPS, 25);
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
    mutable unsigned long long count_;
    mutable long double sec_;
    mutable timer<high_resolution_clock> timer_;
    mutable boost::chrono::high_resolution_clock::time_point read_time_;
};

} /* namespace vidstream */

#endif // CAMERA_HPP__

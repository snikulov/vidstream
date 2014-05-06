#ifndef CAMERA_HPP__
#define CAMERA_HPP__

#include <string>
#include <exception>
#include <boost/shared_ptr.hpp>

#include <opencv2/opencv.hpp>

using cv::VideoCapture;

namespace vidstream {

typedef boost::shared_ptr<cv::Mat> camera_frame_t;

class camera
{
public:
    camera() :
        src_(new VideoCapture()), fname_(), is_hw_cam_(true)
    {
        open();
    }

    camera(const std::string& fname) :
        src_(new VideoCapture()), fname_(fname), is_hw_cam_(false)
    {
        open();
    }

    ~camera()
    {
    }

    camera_frame_t get_frame() const
    {
        camera_frame_t ret_val(new cv::Mat());
        if (src_->read(*ret_val))
        {
            return ret_val;
        }
        return camera_frame_t();
    }

private:
    // init camera
    void open()
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

        width_  = src_->get(CV_CAP_PROP_FRAME_WIDTH);
        height_ = src_->get(CV_CAP_PROP_FRAME_HEIGHT);
    }
    /* data */
    boost::scoped_ptr<VideoCapture> src_;
    std::string fname_;
    bool is_hw_cam_;
    double height_;
    double width_;
};

} /* namespace vidstream */

#endif // CAMERA_HPP__

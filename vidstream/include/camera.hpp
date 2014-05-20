#ifndef CAMERA_HPP__
#define CAMERA_HPP__

#include "frame.hpp"
#include "cfg/cfg_notify.hpp"

#include <string>
#include <exception>

#include <opencv2/opencv.hpp>

using cv::VideoCapture;

namespace vidstream
{

class camera : public cfg_notify
{
public:
    camera(int w = 640, int h = 480) :
        src_(new VideoCapture()), fname_(), is_hw_cam_(true), req_size_(cv::Size(w,h))
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
        camera_frame_t ret_val(new cv::Mat());
        if (src_->read(*ret_val))
        {
            if(ret_val->size() != req_size_)
            {
                cv::resize(*ret_val, *ret_val, req_size_);
            }
            return ret_val;
        }
        ret_val.reset();
        return ret_val;
    }

    void cfg_changed(const boost::property_tree::ptree& cfg)
    {
        int w = cfg.get<int>("cfg.img.width");
        int h = cfg.get<int>("cfg.img.height");
        bool bw = cfg.get<bool>("cfg.img.bw");

        cv::Size tmp(w, h);
        if (req_size_ != tmp)
        {
            req_size_ = tmp;
        }
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

// TODO: should reconsider to set hw capture prop instead of resize
//        width_  = src_->get(CV_CAP_PROP_FRAME_WIDTH);
//        height_ = src_->get(CV_CAP_PROP_FRAME_HEIGHT);
    }
    /* data */
    boost::scoped_ptr<VideoCapture> src_;
    std::string fname_;
    bool is_hw_cam_;
    cv::Size req_size_;
};

} /* namespace vidstream */

#endif // CAMERA_HPP__

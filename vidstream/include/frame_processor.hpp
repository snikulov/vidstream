#ifndef FRAME_PROCESSOR_HPP__
#define FRAME_PROCESSOR_HPP__

#include <monitor_queue.hpp>
#include <transport/transport.hpp>
#include <jpeg/jpeg_builder.hpp>
#include <jpeg/jpeg_transport.hpp>

namespace vidstream
{

class frame_processor
    : public cfg_notify
{
public:
    frame_processor(const cv::Size& sz, monitor_queue<camera_frame_t>& q, int& stop_flag,
                    const std::string& url, boost::shared_ptr<jpeg_builder> jb
#if defined(BUILD_FOR_LINUX)
                    , boost::shared_ptr<ecc> ecc
#endif
                   )
        : req_size_(new cv::Size(sz)), q_(q), stop_(stop_flag), url_(url), jb_(jb)
#if defined(BUILD_FOR_LINUX)
        , ecc_(ecc)
#endif
    {
    }

    void operator()() const
    {
        cv::namedWindow("Capture",1);

        boost::shared_ptr<transport> trans;
        if (url_.size() != 0)
        {
            try
            {
                trans.reset(new transport(TRANSPORT_PUSH, url_));
            }
            catch(nn::exception& ex)
            {
                std::cerr << "Error initializing transport: " << ex.what() << std::endl;
                trans.reset();
            }
        }
        int max_err_try = 0;
        while(!stop_)
        {
            camera_frame_t frame = q_.dequeue();
            if (frame && !frame->empty())
            {
                // resize if needed
                if(frame->size() != *req_size_)
                {
                    cv::resize(*frame, *frame, *req_size_);
                }
                // process incoming frame from camera
                cv::imshow("Capture", *frame);

                // pack frame into jpeg with rst
                jpeg_data_t     jpg(jb_->from_cvmat(frame));
                jpeg_rst_idxs_t rst(jb_->rst_idxs(jpg));

                if (trans)
                {
                    try
                    {
                        int ret = jpeg_transport::send_jpeg(jpg, rst, trans, ecc_);
                        if (ret == -1)
                        {
                            //    std::cerr << "Error send jpeg... Skip" << std::endl;
                            max_err_try++;
                            if (max_err_try > 10)
                            {
                                std::cerr << "Error send jpeg..." << std::endl;
                                trans.reset(new transport(TRANSPORT_PUSH, url_));
                                max_err_try = 0;
                            }
                        }
                        else
                        {
                            max_err_try = 0;
                        }
                    }
                    catch(nn::exception& ex)
                    {
                        std::cerr << "Error sending jpeg: " << ex.what()
                                  << " closing transport" << std::endl;
                        // close transport - TODO: think how to reconnect
                        trans.reset(new transport(TRANSPORT_PUSH, url_));
                        max_err_try = 0;

                    }
                }
            }
            if(cv::waitKey(30) >= 0) stop_ = true;
        }
    }

    ~frame_processor()
    {
    }

    void cfg_changed(const boost::property_tree::ptree& cfg)
    {
        int w = cfg.get<int>("cfg.img.width");
        int h = cfg.get<int>("cfg.img.height");
        bool bw = cfg.get<bool>("cfg.img.bw");

        cv::Size tmp(w, h);
        if (*req_size_ != tmp)
        {
            *req_size_ = tmp;
        }
    }

private:
    /* data */
    boost::shared_ptr<cv::Size> req_size_;
    monitor_queue<camera_frame_t>& q_;
    int& stop_;
    std::string url_;
    boost::shared_ptr<jpeg_builder> jb_;

#if defined(BUILD_FOR_LINUX)
    boost::shared_ptr<ecc> ecc_;
#endif

};

} /* namespace vidstream */

#endif // FRAME_PROCESSOR_HPP__


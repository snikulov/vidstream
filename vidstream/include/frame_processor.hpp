#ifndef FRAME_PROCESSOR_HPP__
#define FRAME_PROCESSOR_HPP__

#include <monitor_queue.hpp>
#include <transport/transport.hpp>
#include <jpeg/jpeg_builder.hpp>
#include <jpeg/jpeg_transport.hpp>

#include <perf/perf_clock.hpp>

#include <boost/smart_ptr.hpp>


namespace vidstream
{

class frame_processor
    : public cfg_notify
{
public:
    frame_processor(const cv::Size& sz, monitor_queue<camera_frame_t>& q, int& stop_flag,
                    const std::string& url, boost::shared_ptr<jpeg_builder> jb
                    , stat_data_t * stat, bchwrapper& codec
                   )
        : req_size_(new cv::Size(sz)), is_bw_(new bool(false)), q_(q), stop_(stop_flag), url_(url), jb_(jb)
        , cnt_processed_(0), cnt_sent_(0), stat_(stat), codec_(codec)
    {
    }

    void operator()() const
    {
#if defined(CAPTURE_UI)
        cv::namedWindow("Capture",1);
#endif
        boost::shared_ptr<jpeg_transport> jpgtrans(new jpeg_transport());

        boost::shared_ptr<out_channel> outsink(new out_channel(url_, codec_));

        int max_err_try = 0;

        timer_.start();
        while(!stop_)
        {
            timer<high_resolution_clock> pt;

            camera_frame_t frame = q_.dequeue();
            if (frame && !frame->empty())
            {
                cnt_processed_++;
                // resize if needed
                if(frame->size() != *req_size_)
                {
                    cv::resize(*frame, *frame, *req_size_);
                }

                if (*is_bw_)
                {
                    boost::shared_ptr<cv::Mat> gs(new cv::Mat());
                    cvtColor(*frame, *gs, cv::COLOR_RGB2GRAY);
                    frame.swap(gs);
                }

#if defined(CAPTURE_UI)
                // process incoming frame from camera
                cv::imshow("Capture", *frame);
#endif
                // pack frame into jpeg with rst
                jpeg_data_t     jpg(jb_->from_cvmat(frame));
                jpeg_rst_idxs_t rst(jb_->rst_idxs(jpg));

                pt.stop();
                stat_->f_process_time_ = pt.sec();

                if (outsink)
                {
                    try
                    {
                        pt.start();
                        int ret = jpgtrans->send_jpeg(jpg, rst, outsink);

                        if (ret == -1)
                        {
                            //    std::cerr << "Error send jpeg... Skip" << std::endl;
                            max_err_try++;
                            if (max_err_try > 10)
                            {
                                std::cerr << "Error send jpeg..." << std::endl;
                                outsink.reset(new out_channel(url_, codec_));
                                //trans.reset(new transport(TRANSPORT_PUSH, url_));
                                max_err_try = 0;
                            }
                        }
                        else
                        {
                            max_err_try = 0;
                            cnt_sent_++;
                            pt.stop();
//                            std::cerr << "send time: " << pt.seconds() << std::endl;
                            stat_->f_send_time_ = pt.sec();
                        }
                    }
                    catch(nn::exception& ex)
                    {
                        std::cerr << "Error sending jpeg: " << ex.what()
                                  << " closing transport" << std::endl;
                        // close transport - TODO: think how to reconnect
                        // trans.reset(new transport(TRANSPORT_PUSH, url_));
                        outsink.reset(new out_channel(url_, codec_));
                        max_err_try = 0;
                    }
                }
            }
            if(cv::waitKey(10) >= 0)
            {
                stop_ = true;
            }
            timer_.stop();
#if 0
            std::cout << "process FPS: " << get_process_fps()
                      << " sent FPS: " << get_sent_fps()
                      << " sec=" << timer_.sec() << " frame count=" << cnt_processed_ << std::endl;
#endif
            stat_->process_fps_= get_process_fps();
        }
    }

    unsigned int get_process_fps() const
    {
        unsigned long t = timer_.sec();
        if (t > 0)
        {
            return  static_cast<unsigned int>(cnt_processed_/timer_.sec());
        }
        return cnt_processed_;
    }

    unsigned int get_sent_fps() const
    {
        unsigned long t = timer_.sec();
        if (t > 0)
        {
            return  static_cast<unsigned int>(cnt_sent_/timer_.sec());
        }
        return cnt_sent_;
    }


    ~frame_processor()
    {
    }

    void cfg_changed(const boost::property_tree::ptree& cfg)
    {
        int w = cfg.get<int>("cfg.img.width");
        int h = cfg.get<int>("cfg.img.height");
        bool bw = cfg.get<bool>("cfg.img.bw");

        int bch_n = cfg.get<int>("cfg.bch.n");
        int bch_t = cfg.get<int>("cfg.bch.t");

        cv::Size tmp(w, h);
        if (*req_size_ != tmp)
        {
            *req_size_ = tmp;
        }

        if (*is_bw_ != bw)
        {
            *is_bw_ = bw;
        }

        codec_.change_params(bch_n, bch_t);
    }

private:
    /* data */
    boost::shared_ptr<cv::Size> req_size_;
    boost::shared_ptr<bool> is_bw_;

    monitor_queue<camera_frame_t>& q_;
    int& stop_;
    std::string url_;
    boost::shared_ptr<jpeg_builder> jb_;
    bchwrapper& codec_;

    mutable unsigned long long cnt_processed_;
    mutable unsigned long long cnt_sent_;
    mutable timer<high_resolution_clock> timer_;

    stat_data_t * stat_;

};

} /* namespace vidstream */

#endif // FRAME_PROCESSOR_HPP__


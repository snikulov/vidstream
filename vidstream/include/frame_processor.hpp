#ifndef FRAME_PROCESSOR_HPP__
#define FRAME_PROCESSOR_HPP__

#include <monitor_queue.hpp>
#include <transport/transport.hpp>
#include <jpeg/jpeg_builder.hpp>
#include <jpeg/jpeg_transport.hpp>
#include <ecc/bch_codec.hpp>
#include <perf/perf_clock.hpp>


namespace vidstream
{

class frame_processor
    : public cfg_notify
{
public:
    frame_processor(const cv::Size& sz, monitor_queue<camera_frame_t>& q, int& stop_flag,
                    const std::string& url, boost::shared_ptr<jpeg_builder> jb
                    , boost::shared_ptr<bch_codec> ecc
                    , stat_data_t * stat
                   )
        : req_size_(new cv::Size(sz)), q_(q), stop_(stop_flag), url_(url), jb_(jb), ecc_(ecc)
          , cnt_processed_(0), cnt_sent_(0), stat_(stat)
    {
    }

    void operator()() const
    {
#if defined(CAPTURE_UI)
        cv::namedWindow("Capture",1);
#endif
//        boost::shared_ptr<transport> trans;
        boost::shared_ptr<jpeg_transport> jpgtrans(new jpeg_transport());

        boost::shared_ptr<out_channel> outsink(new out_channel(url_, boost::shared_ptr<itpp::Channel_Code>()));
#if 0
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
#endif
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
#if defined(CAPTURE_UI)
                // process incoming frame from camera
                cv::imshow("Capture", *frame);
#endif
                // pack frame into jpeg with rst
                jpeg_data_t     jpg(jb_->from_cvmat(frame));
                jpeg_rst_idxs_t rst(jb_->rst_idxs(jpg));

                pt.stop();
//                std::cerr << "process time: " << pt.seconds() << std::endl;
                stat_->f_process_time_ = pt.seconds();

                if (outsink)
                {
                    try
                    {
                        pt.start();
//                        int ret = jpgtrans->send_jpeg(jpg, rst, trans, ecc_);
                        int ret = jpgtrans->send_jpeg(jpg, rst, outsink);

                        if (ret == -1)
                        {
                            //    std::cerr << "Error send jpeg... Skip" << std::endl;
                            max_err_try++;
                            if (max_err_try > 10)
                            {
                                std::cerr << "Error send jpeg..." << std::endl;
                                outsink.reset(new out_channel(url_, 0));
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
                            stat_->f_send_time_ = pt.seconds();
                        }
                    }
                    catch(nn::exception& ex)
                    {
                        std::cerr << "Error sending jpeg: " << ex.what()
                                  << " closing transport" << std::endl;
                        // close transport - TODO: think how to reconnect
                        // trans.reset(new transport(TRANSPORT_PUSH, url_));
                        outsink.reset(new out_channel(url_, 0));
                        max_err_try = 0;
                    }
                }
            }
#if defined(CAPTURE_UI)
            if(cv::waitKey(10) >= 0)
            {
                stop_ = true;
            }
#endif
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
        return  static_cast<unsigned int>(cnt_processed_/timer_.seconds());
    }

    unsigned int get_sent_fps() const
    {
        return  static_cast<unsigned int>(cnt_sent_/timer_.seconds());
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
    boost::shared_ptr<bch_codec> ecc_;
    mutable unsigned long long cnt_processed_;
    mutable unsigned long long cnt_sent_;
    mutable timer<high_resolution_clock> timer_;

    stat_data_t * stat_;

};

} /* namespace vidstream */

#endif // FRAME_PROCESSOR_HPP__


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
      , private boost::noncopyable
{
public:
    frame_processor(const cv::Size& sz, monitor_queue<camera_frame_t>& q, int& stop_flag,
                    const std::string& url, boost::shared_ptr<jpeg_builder> jb
                    , stat_data_t * stat, bchwrapper& codec
                    , int bw_limit = 10, int fps_limit = 25, int quality = 100
                   )
        : req_size_(new cv::Size(sz)), is_bw_(new bool(false)), q_(q)
          , stop_(stop_flag), url_(url), jb_(jb)
          , cnt_processed_(0), cnt_sent_(0), stat_(stat), codec_(codec)
          , bw_limit_(bw_limit), fps_limit_(fps_limit), jpg_quality_(quality)
    {
    }

    void operator()() const
    {
        boost::shared_ptr<jpeg_transport> jpgtrans(new jpeg_transport());

        boost::shared_ptr<out_channel> outsink(new out_channel(url_, codec_, stat_));

        int max_err_try = 0;
        timer<high_resolution_clock> pt;

        while(!stop_)
        {
            pt.restart();

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
                    boost::shared_ptr<cv::Mat> gs( new cv::Mat() );
                    cvtColor(*frame, *gs, cv::COLOR_RGB2GRAY);
                    frame.swap(gs);
                }

                // pack frame into jpeg with rst
                jpeg_data_t     jpg(jb_->from_cvmat(frame));
                jpeg_rst_idxs_t rst(jb_->rst_idxs(jpg));

                if (outsink)
                {
                    try
                    {
                        size_t data_size = jpgtrans->send_jpeg(jpg, rst, outsink);

                        // here we know size of frame
                        recalculate_jpeg_quality(data_size);

                    }
                    catch(nn::exception& ex)
                    {
                        outsink.reset(new out_channel(url_, codec_, stat_));
                        max_err_try = 0;
                    }
                }

                stat_->f_process_time_ = pt.nsec();
                stat_->frame_size_ = jpg->size();
                stat_->num_rst_ = rst->size();
                stat_->ecc_payload_coef_ = codec_.get_encode_coef();
           }

            stat_->process_fps_= get_process_fps();
        }
    }

    unsigned int get_process_fps() const
    {
        unsigned long long t = timer_.sec();
        if (t > 0)
        {
            return  static_cast<unsigned int>(cnt_processed_/timer_.sec());
        }
        return cnt_processed_;
    }

    unsigned int get_sent_fps() const
    {
        unsigned long long t = timer_.sec();
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

        bw_limit_ = cfg.get<int>("cfg.bw");
        fps_limit_ = cfg.get<int>("cfg.fps.lim");
        jpg_quality_ = cfg.get<int>("cfg.img.q");

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

    void recalculate_jpeg_quality(size_t frame_size) const
    {
        unsigned long bch_size_in_byte =
            static_cast<unsigned long>(
                    std::ceil(static_cast<double>(frame_size) * codec_.get_encode_coef())
                    );
        unsigned long bw_in_bytes = (bw_limit_ * 1000000UL) / 8;

        unsigned long expected_frame_size = bw_in_bytes / fps_limit_;

        if (bch_size_in_byte > expected_frame_size)
        {
            // lowering quality
            if (jpg_quality_ > 22)
            {
                jpg_quality_ -= 2;
            }
        }
        else
        {
            // increase quality
            if (jpg_quality_ < 100)
            {
                jpg_quality_ +=1;
            }
        }
        jb_->set_quality(jpg_quality_);
        stat_->jpeg_auto_quality_ = jpg_quality_;
    }

    /* data */
    boost::shared_ptr<cv::Size> req_size_;
    boost::shared_ptr<bool> is_bw_;

    monitor_queue<camera_frame_t>& q_;
    int& stop_;
    std::string url_;
    boost::shared_ptr<jpeg_builder> jb_;
    bchwrapper& codec_;

    int bw_limit_;
    int fps_limit_;
    mutable int jpg_quality_;

    mutable unsigned long long cnt_processed_;
    mutable unsigned long long cnt_sent_;
    mutable timer<high_resolution_clock> timer_;

    stat_data_t * stat_;

};

} /* namespace vidstream */

#endif // FRAME_PROCESSOR_HPP__


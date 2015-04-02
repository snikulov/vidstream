#ifndef FRAME_PRODUCER_HPP__
#define FRAME_PRODUCER_HPP__

#include <camera.hpp>
#include <monitor_queue.hpp>
#include <stat/stat_data.hpp>

#include <boost/ratio.hpp>

namespace vidstream
{

    class frame_producer
        : public cfg_notify
          , private boost::noncopyable
    {
        public:
            frame_producer(const camera& cam, monitor_queue<camera_frame_t>& q,
                   int& stop_flag, stat_data_t * stat, int frame_rate = 25)
                : cam_(cam), frame_rate_(frame_rate), sec_per_frame_(1.0/frame_rate_)
                , q_(q), stop_(stop_flag), stat_(stat)
            {
                ns_per_frame_ = recalculate_ns_per_frame(sec_per_frame_);
            }

            void operator()() const
            {
                log4cplus::Logger log_ = log4cplus::Logger::getInstance("producer_thread");

                LOG4CPLUS_INFO(log_, "Frame producer thread started...");

                camera_frame_t frame;
                unsigned long long sec = 1;
                unsigned long long fcount = 0;

                timer<boost::chrono::steady_clock> t1;
                timer<boost::chrono::steady_clock> t2;

                while(!stop_)
                {
                    t2.restart();
                    frame = cam_.get_frame();
                    fps_rate_limit(t2.elapsed());

                    if (frame)
                    {
                        fcount++;
                        q_.enqueue(frame);
                    }
                    else
                    {
                        LOG4CPLUS_WARN(log_, "Camera return empty frame: " << fcount);
                    }

                    sec = t1.sec();
                    sec = sec ? sec : 1; // 0 div check

                    unsigned long long fps = fcount/sec;
                    stat_->cam_fps_ = fps;
                    stat_->tsec_ = sec;
                }

                LOG4CPLUS_INFO(log_, "Frame producer exited...");
            }

            ~frame_producer()
            {
            }

            void cfg_changed(const boost::property_tree::ptree& cfg)
            {
                int fps_lim = cfg.get<int>("cfg.fps.lim");
                if (frame_rate_ != fps_lim)
                {
                    boost::mutex::scoped_lock lk(mx_);
                    frame_rate_ = fps_lim;
                    sec_per_frame_ = 1.0/frame_rate_;
                    ns_per_frame_ = recalculate_ns_per_frame(sec_per_frame_);
                }
            }


        private:

            boost::chrono::nanoseconds recalculate_ns_per_frame(double sec_pf)
            {
                boost::chrono::duration<double> spf(sec_pf);
                return boost::chrono::duration_cast<boost::chrono::nanoseconds>(spf);
            }

            void fps_rate_limit(const boost::chrono::steady_clock::duration& frame_get_time) const
            {
                if (frame_get_time < ns_per_frame_)
                {
                     boost::this_thread::sleep_for(ns_per_frame_ - frame_get_time);
                }
            }

            /* data */
            const camera& cam_;
            int  frame_rate_;
            double sec_per_frame_;
            boost::mutex mx_;
            monitor_queue<camera_frame_t>& q_;
            int& stop_;
            stat_data_t * stat_;

            boost::chrono::nanoseconds ns_per_frame_;
    };

} /* namespace vidstream */

#endif // FRAME_PRODUCER_HPP__


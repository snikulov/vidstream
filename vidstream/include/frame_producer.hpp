#ifndef FRAME_PRODUCER_HPP__
#define FRAME_PRODUCER_HPP__

#include <camera.hpp>
#include <monitor_queue.hpp>
#include <stat/stat_data.hpp>

namespace vidstream
{

    class frame_producer
    {
        public:
            frame_producer(const camera& cam, monitor_queue<camera_frame_t>& q,
                   int& stop_flag, stat_data_t * stat)
                : cam_(cam), q_(q), stop_(stop_flag), stat_(stat)
            {
            }

            void operator()() const
            {
                log4cplus::Logger log_ = log4cplus::Logger::getInstance("producer_thread");

                LOG4CPLUS_INFO(log_, "Frame producer thread started...");

                // 1/25 seconds - time to get frame
                boost::chrono::duration<unsigned int, boost::ratio<1, 25> > time_per_frame(1);
                boost::chrono::nanoseconds ns_per_frame = boost::chrono::duration_cast<boost::chrono::nanoseconds>(time_per_frame);

                timer<boost::chrono::steady_clock> t1;
                timer<boost::chrono::steady_clock> t2;

                camera_frame_t frame;
                unsigned long long sec = 1;
                unsigned long long fcount = 0;

                t1.start();

                while(!stop_)
                {
                    t2.start();
                    frame = cam_.get_frame();
                    t2.stop();
                    if(t2.elapsed() < ns_per_frame)
                    {
                        boost::this_thread::sleep_for(ns_per_frame - t2.elapsed());
                    }

                    LOG4CPLUS_TRACE(log_, "capture ns: " << t2.nsec());

                    if (frame)
                    {
                        fcount++;
                        t2.start();
                        q_.enqueue(frame);
                        t2.stop();
                        LOG4CPLUS_TRACE(log_, "push to processing ns: " << t2.nsec());

                    }
                    else
                    {
                        LOG4CPLUS_WARN(log_, "Camera return empty frame: " << fcount << ", " << sec);
                    }

                    t1.stop();
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

        private:
            /* data */
            const camera& cam_;
            monitor_queue<camera_frame_t>& q_;
            int& stop_;
            stat_data_t * stat_;
    };

} /* namespace vidstream */

#endif // FRAME_PRODUCER_HPP__


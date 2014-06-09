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
    frame_producer(const camera& cam, monitor_queue<camera_frame_t>& q, int& stop_flag, stat_data_t * stat)
        : cam_(cam), q_(q), stop_(stop_flag), stat_(stat)
    {
    }

    void operator()() const
    {
        while(!stop_)
        {
            q_.enqueue(cam_.get_frame());
            stat_->cam_fps_ = cam_.get_soft_fps();
        }
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


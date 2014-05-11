#ifndef FRAME_PROCESSOR_HPP__
#define FRAME_PROCESSOR_HPP__

#include <monitor_queue.hpp>
#include <jpeg_builder.hpp>

namespace vidstream
{

typedef struct
{
    std::vector<unsigned char> buf;
    std::vector<unsigned int> rst_marks;
} data_holder_t;

class frame_processor
{
public:
    frame_processor(monitor_queue<camera_frame_t>& q, int& stop_flag)
        : q_(q), stop_(stop_flag)
    {
    }

    void operator()() const
    {
        cv::namedWindow("Capture",1);
        jpeg_builder jpeg;

        while(!stop_)
        {
            camera_frame_t frame = q_.dequeue();
            if (frame && !frame->empty())
            {
                // process incoming frame from camera
                cv::imshow("Capture", *frame);

                // pack frame into jpeg with rst
                jpeg_data_t d = jpeg.from_cvmat(frame);
            }
            else
            {
                boost::thread::yield();
            }
            if(cv::waitKey(30) >= 0) stop_ = true;
        }
    }

    ~frame_processor()
    {
    }

private:
    /* data */
    monitor_queue<camera_frame_t>& q_;
    int& stop_;

};

} /* namespace vidstream */

#endif // FRAME_PROCESSOR_HPP__


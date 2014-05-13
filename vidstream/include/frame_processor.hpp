#ifndef FRAME_PROCESSOR_HPP__
#define FRAME_PROCESSOR_HPP__

#include <monitor_queue.hpp>
#include <jpeg_builder.hpp>
#include <transport.hpp>

#if defined(BUILD_FOR_LINUX)
#include <ecc/ecc.h>
#endif

namespace vidstream
{

class frame_processor
{
public:
    frame_processor(monitor_queue<camera_frame_t>& q, int& stop_flag,
           const std::string& url
#if defined(BUILD_FOR_LINUX)
           , boost::shared_ptr<ecc> ecc
#endif
           )
        : q_(q), stop_(stop_flag), url_(url)
#if defined(BUILD_FOR_LINUX)
        , ecc_(ecc)
#endif
    {
    }

    void operator()() const
    {
        cv::namedWindow("Capture",1);
        jpeg_builder jbuilder;

        boost::scoped_ptr<transport> trans;
        if (url_.size() != 0)
        {
#if defined(BUILD_FOR_LINUX)
            trans.reset(new transport(url_, ecc_));
#else
            trans.reset(new transport(url_));
#endif
        }

        while(!stop_)
        {
            camera_frame_t frame = q_.dequeue();
            if (frame && !frame->empty())
            {
                // process incoming frame from camera
                cv::imshow("Capture", *frame);

                // pack frame into jpeg with rst
                jpeg_data_t     jpg(jbuilder.from_cvmat(frame));
                jpeg_rst_idxs_t rst(jbuilder.rst_idxs(jpg));

                if (trans)
                {
                    trans->send_jpeg(jpg, rst);
                }
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
    std::string url_;

#if defined(BUILD_FOR_LINUX)
    boost::shared_ptr<ecc> ecc_;
#endif

};

} /* namespace vidstream */

#endif // FRAME_PROCESSOR_HPP__


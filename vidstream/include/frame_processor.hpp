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
           const std::string& url, boost::shared_ptr<jpeg_builder> jb
#if defined(BUILD_FOR_LINUX)
           , boost::shared_ptr<ecc> ecc
#endif
           )
        : q_(q), stop_(stop_flag), url_(url), jb_(jb)
#if defined(BUILD_FOR_LINUX)
        , ecc_(ecc)
#endif
    {
    }

    void operator()() const
    {
        cv::namedWindow("Capture",1);

        boost::scoped_ptr<transport> trans;
        if (url_.size() != 0)
        {
            try
            {
#if defined(BUILD_FOR_LINUX)
                trans.reset(new transport(TRANSPORT_PUSH, url_, ecc_));
#else
                trans.reset(new transport(TRANSPORT_PUSH, url_));
#endif
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
                // process incoming frame from camera
                cv::imshow("Capture", *frame);

                // pack frame into jpeg with rst
                jpeg_data_t     jpg(jb_->from_cvmat(frame));
                jpeg_rst_idxs_t rst(jb_->rst_idxs(jpg));

                if (trans)
                {
                    try
                    {
                        int ret = trans->send_jpeg(jpg, rst);
                        if (ret == -1)
                        {
                        //    std::cerr << "Error send jpeg... Skip" << std::endl;
                            max_err_try++;
                            if (max_err_try > 10)
                            {
                                std::cerr << "Error send jpeg..." << std::endl;
#if defined(BUILD_FOR_LINUX)
                                trans.reset(new transport(TRANSPORT_PUSH, url_, ecc_));
#else
                                trans.reset(new transport(TRANSPORT_PUSH, url_));
#endif
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
#if defined(BUILD_FOR_LINUX)
                        trans.reset(new transport(TRANSPORT_PUSH, url_, ecc_));
#else
                        trans.reset(new transport(TRANSPORT_PUSH, url_));
#endif
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

private:
    /* data */
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


#ifndef OCV_OUTPUT_HPP__
#define OCV_OUTPUT_HPP__

#include <monitor_queue.hpp>
#include <opencv2/opencv.hpp>

class ocv_output
{
public:
    ocv_output(bool& stop, const std::string& winname, monitor_queue<cv::Mat>& mq)
        : stop_(stop), winname_(winname), mq_(mq)
    {
    }
    ~ocv_output()
    {
    }

    void show(cv::Mat to_display)
    {
        mq_.enqueue(to_display);
    }

    void operator()()
    {
        cv::namedWindow(winname_);

        while(!stop_)
        {
            cv::Mat pic = mq_.dequeue();

            if (!pic.empty())
            {
                cv::imshow(winname_, pic);
            }
            {
                std::cout << "got empty" << std::endl;
            }
            cv::waitKey(30);
        }
    }

    void stop()
    {
        stop_ = true;
        cv::Mat empty;
        show(empty);
    }

private:
    /* data */
    bool& stop_;
    std::string winname_;

    monitor_queue<cv::Mat>& mq_;
};
#endif


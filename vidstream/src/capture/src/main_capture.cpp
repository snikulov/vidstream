#include <iostream>
#include <boost/scoped_ptr.hpp>
#include <camera.hpp>

#include <frame_producer.hpp>

#include <opencv2/opencv.hpp>

using vidstream::camera;
using vidstream::camera_frame_t;
using vidstream::frame_producer;

int main(int argc, char** argv)
{

    boost::scoped_ptr<camera> cam(argc > 1 ? new camera(argv[1]) : new camera());
    monitor_queue<camera_frame_t> mq(10);
    camera& c = *cam;
    int stop_flag = 0;
    frame_producer fp(c, mq, stop_flag);

    boost::thread t(fp);

    cv::namedWindow("Capture",1);
    for(;;)
    {
        camera_frame_t frame = mq.dequeue();
        if (frame)
            cv::imshow("Capture", *frame);
        if(cv::waitKey(30) >= 0) break;
    }
    stop_flag = 1;
    t.join();

    return 0;
}



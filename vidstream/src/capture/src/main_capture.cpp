#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <vector>
#include <boost/scoped_ptr.hpp>

#include <opencv2/opencv.hpp>

#include <camera.hpp>

#include <frame_producer.hpp>
#include <frame_processor.hpp>


using vidstream::camera;
using vidstream::camera_frame_t;
using vidstream::frame_producer;
using vidstream::frame_processor;

int main(int argc, char** argv)
{
    boost::scoped_ptr<camera> cam(argc > 1 ? new camera(argv[1]) : new camera());
    monitor_queue<camera_frame_t> mq(10);
    camera& c = *cam;
    int stop_flag = 0;

    frame_producer producer(c, mq, stop_flag);
    frame_processor processor(mq, stop_flag);

    boost::thread tproducer(producer);
    boost::thread tprocess(processor);

    tproducer.join();
    tprocess.join();

    return 0;
}



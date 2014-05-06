#include <iostream>
#include <boost/scoped_ptr.hpp>
#include <camera.hpp>

#include <opencv2/opencv.hpp>

using vidstream::camera;
using vidstream::camera_frame_t;

int main(int argc, char** argv)
{
    boost::scoped_ptr<camera> cam(argc > 1 ? new camera(argv[1]) : new camera());
    camera& c = *cam;

    cv::namedWindow("Capture",1);
    for(;;)
    {
        camera_frame_t frame = c.get_frame();
        if (frame)
            cv::imshow("Capture", *frame);
        if(cv::waitKey(30) >= 0) break;
    }
    return 0;
}



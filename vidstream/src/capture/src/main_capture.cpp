#include <memory>
#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;

int main(int argc, char** argv)
{
    std::unique_ptr<VideoCapture> cap;
    if (argc > 1) {
        cap.reset(new VideoCapture(argv[1]));
    }
    else {
        cap.reset(new VideoCapture(0));
    }

    if(!cap->isOpened())  // check if we succeeded
        return -1;

    std::cout << "Capture width = " << cap->get(CV_CAP_PROP_FRAME_WIDTH) 
              << " height = " << cap->get(CV_CAP_PROP_FRAME_HEIGHT) << std::endl;

    if(!cap->set(CV_CAP_PROP_FRAME_WIDTH, 640)) std::cout << "failed to set width" << std::endl;
    if(!cap->set(CV_CAP_PROP_FRAME_HEIGHT, 480))  std::cout << "failed to set height" << std::endl;

    Size size_(cap->get(CV_CAP_PROP_FRAME_WIDTH)/2, cap->get(CV_CAP_PROP_FRAME_HEIGHT)/2);

    namedWindow("Capture",1);
    Mat frame;
    for(;;)
    {
        if(cap->read(frame)) {
            resize(frame, frame, size_);
            imshow("Capture", frame);
        }
        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}



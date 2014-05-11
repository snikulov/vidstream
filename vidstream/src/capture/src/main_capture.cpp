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
#if 0
    std::vector<int> param;
    param.push_back(CV_IMWRITE_JPEG_QUALITY);
    param.push_back(100);
    param.push_back(CV_IMWRITE_JPEG_RST_INTERVAL);
    param.push_back(1);
    param.push_back(CV_IMWRITE_JPEG_LUM_QUALITY);
    param.push_back(20);
    param.push_back(CV_IMWRITE_JPEG_CHROM_QUALITY);
    param.push_back(20);


    static unsigned long name_count = 0;
    bool cnf_write_frame_to_file = false;

    for(;;)
    {
        camera_frame_t frame = mq.dequeue();
        if(frame)
        {
            std::vector<unsigned char> buf;
            bool res = cv::imencode(".jpg", *frame, buf, param);
            if (res && cnf_write_frame_to_file)
            {
                // TODO: write file -  move to out module
                std::ostringstream fn;
                fn << "img" << std::setfill ('0') << std::setw(8) << name_count++ << ".jpg";
                std::ofstream o(fn.str().c_str(), std::ios_base::binary);
                o.write(reinterpret_cast<const char*>(&buf[0]), buf.size()*sizeof(unsigned char));
                o.close();
            }

            cv::imshow("Capture", *frame);
        }
        else
        {
            boost::thread::yield();
        }
        if(cv::waitKey(30) >= 0) break;
    }
    stop_flag = 1;
#endif
    tproducer.join();
    tprocess.join();

    return 0;
}



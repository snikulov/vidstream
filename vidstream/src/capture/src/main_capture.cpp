#include <iostream>
#include <stdexcept>

#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/program_options.hpp>

#include <opencv2/opencv.hpp>

#include <camera.hpp>

#include <frame_producer.hpp>
#include <frame_processor.hpp>


using vidstream::camera;
using vidstream::camera_frame_t;
using vidstream::frame_producer;
using vidstream::frame_processor;

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    po::options_description desc("All options");
    desc.add_options()
        ("width,w", po::value<int>()->default_value(640), "video width")
        ("height,h", po::value<int>()->default_value(480), "video height")
        ("url,u", po::value<std::string>()->default_value("tcp://127.0.0.1:9999"),
            "url for publising data {tcp://<local IP>:<local PORT>}")
        ("file,f", po::value<std::string>()->default_value(""), "path to input video file")
        ("bm", po::value<unsigned char>()->default_value(5), "BCH m")
        ("bt", po::value<unsigned char>()->default_value(4), "BCH t")
        ("help,?", "produce help message");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        // print help and exit
        std::cout << desc << "\n";
        return 1;
    }

    int w = vm["width"].as<int>();
    int h = vm["height"].as<int>();
    std::string infile = vm["file"].as<std::string>();
    std::string url = vm["url"].as<std::string>();
    unsigned char bm = vm["bm"].as<unsigned char>();
    unsigned char bt = vm["bt"].as<unsigned char>();

    boost::shared_ptr<ecc> bch_ecc(new ecc(bm, bt));

    boost::scoped_ptr<camera> cam(infile.size() == 0
            ? new camera(w,h)
            : new camera(infile, w, h));

    monitor_queue<camera_frame_t> mq(10);
    camera& c = *cam;
    int stop_flag = 0;

    frame_producer producer(c, mq, stop_flag);
    frame_processor processor(mq, stop_flag, url, bch_ecc);

    boost::thread tproducer(producer);
    boost::thread tprocess(processor);

    tproducer.join();
    tprocess.join();

    return 0;
}



#include <iostream>
#include <stdexcept>
#include <vector>

#include <boost/scoped_ptr.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <opencv2/opencv.hpp>

#include <camera.hpp>

#include <frame_producer.hpp>
#include <frame_processor.hpp>

#include "cfg/cfg_subscribers.hpp"

using namespace vidstream;

namespace po = boost::program_options;


void display(const int depth, const boost::property_tree::ptree& tree)
{
    using namespace boost::property_tree;
    using namespace std;

    BOOST_FOREACH( ptree::value_type const&v, tree.get_child("") ) {
        ptree subtree = v.second;
        string nodestr = tree.get<string>(v.first);

        // print current node
        cout << string("").assign(depth*2,' ') << "* ";
        cout << v.first;
        if ( nodestr.length() > 0 )
            cout << "=\"" << tree.get<string>(v.first) << "\"";
        cout << endl;

        // recursive go down the hierarchy
        display(depth+1,subtree);
    }
}


class cfg_sync_thread : public boost::noncopyable
{
public:
    cfg_sync_thread(int& stop, const std::string& url, boost::shared_ptr<boost::property_tree::ptree> cfg)
        : stop_(stop), url_(url), cfg_(cfg), is_ready_(false)
    {}

    void operator()()
    {
        int err_count = 0;
        trans_.reset(new transport(TRANSPORT_REQ, url_));
        while(!stop_)
        {
            if (err_count >= 10)
            {
                // close
                trans_.reset();
                // re-connect to host after the one second
                boost::this_thread::sleep_for(boost::chrono::seconds(1));
                if(stop_) break;
                err_count = 0;
                trans_.reset(new transport(TRANSPORT_REQ, url_));
            }

            int ret = -1;
            try
            {
                do
                {
                    if(stop_) break;
                    ret = trans_->send("getconfig");
                    boost::this_thread::sleep_for(boost::chrono::milliseconds(250));
                    if (-1 == ret) err_count++;
                    else err_count = 0;
                }while(-1 == ret && err_count < 10 && !stop_);

                if (err_count >= 10 || stop_) continue;

                std::vector<unsigned char> cfgdata;
                do
                {
                    if(stop_) break;
                    ret = trans_->receive(cfgdata);
                    boost::this_thread::sleep_for(boost::chrono::milliseconds(250));
                    if (-1 == ret) err_count++;
                    else err_count = 0;
                }while(-1 == ret && err_count < 10 && !stop_);

                if (err_count >= 10 || stop_) continue;

                // done - got data
                std::string config(cfgdata.begin(), cfgdata.end());
                std::stringstream ss(config);
                boost::property_tree::ptree rhs;
                boost::property_tree::read_json(ss, rhs);
                if (*cfg_ != rhs)
                {
                    cfg_->swap(rhs);
                    subs_.notify_change(*cfg_);
                }
                is_ready_ = true;
                // will repeat each 2 sec
                boost::this_thread::sleep_for(boost::chrono::seconds(2));
                err_count = 0;
            }
            catch(nn::exception& ex)
            {
                std::cerr << "Transport error: " << ex.what() << std::endl;
                err_count = 10; // go to re-connect immediately
                continue;
            }
        }
    }

    bool ready() {return is_ready_;}

    bool subscribe(cfg_notify* sub)
    {
        return subs_.subscribe(sub);
    }

private:
    int & stop_;
    std::string url_;
    boost::shared_ptr<boost::property_tree::ptree> cfg_;
    boost::shared_ptr<transport> trans_;
    bool is_ready_;

    cfg_subscribers subs_;
};

int main(int argc, char** argv)
{
    po::options_description desc("All options");
    desc.add_options()
        ("url,u", po::value<std::string>()->default_value("tcp://127.0.0.1:9900"),
            "url for publising data {tcp://<control IP>:<control PORT>}")
        ("file,f", po::value<std::string>()->default_value(""), "path to input video file")
        ("help,?", "show help");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        // print help and exit
        std::cout << desc << "\n";
        return 1;
    }

    std::string infile = vm["file"].as<std::string>();
    std::string cmd_url = vm["url"].as<std::string>();

    int stop_flag = 0;

    boost::shared_ptr<boost::property_tree::ptree> cfg(new boost::property_tree::ptree());
    cfg_sync_thread resync(stop_flag, cmd_url, cfg);
    boost::thread cfg_thread(boost::ref(resync));

    while(!resync.ready())
    {
        std::cerr << "Configuration is not in sync with server. Retrying..." << std::endl;
        boost::this_thread::sleep_for(boost::chrono::seconds(1));
    }

    display(3, *cfg);

    int bm = cfg->get<int>("cfg.bch.m");
    int bt = cfg->get<int>("cfg.bch.t");
    int w = cfg->get<int>("cfg.img.width");
    int h = cfg->get<int>("cfg.img.height");

    cv::Size isize(w,h);

    std::string dataurl("tcp://127.0.0.1:");
    dataurl += cfg->get<std::string>("cfg.dataport");

    boost::scoped_ptr<camera> cam(infile.size() == 0
            ? new camera(w, h)
            : new camera(infile, w, h));

    boost::shared_ptr<jpeg_builder> jb(new jpeg_builder());

    monitor_queue<camera_frame_t> mq(5);
    camera& c = *cam;
    // subscribe on updates
    resync.subscribe(jb.get());

    boost::shared_ptr<bch_codec> bch_ecc(new bch_codec(bm, bt));
    resync.subscribe(bch_ecc.get());


    frame_producer producer(c, mq, stop_flag);
    frame_processor processor(isize, mq, stop_flag, dataurl
            , jb, bch_ecc);
    resync.subscribe(&processor);

    boost::thread tproducer(producer);
    boost::thread tprocess(processor);

    cfg_thread.join();
    tproducer.join();
    tprocess.join();

    return 0;
}



#include <iostream>
#include <stdexcept>
#include <vector>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/loglevel.h>
#include <log4cplus/configurator.h>


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

#include <cfg/cfg_subscribers.hpp>
#include <stat/stat_data.hpp>

#include <channel/bchwrapper.hpp>

using namespace vidstream;

namespace po = boost::program_options;


void display(const int depth, const boost::property_tree::ptree& tree)
{
    using namespace boost::property_tree;
    using namespace std;

    BOOST_FOREACH( ptree::value_type const&v, tree.get_child("") )
    {
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
        cfg_sync_thread(int& stop, const std::string& url,
                boost::shared_ptr<boost::property_tree::ptree> cfg, stat_data_t* stat)
            : stop_(stop), url_(url), cfg_(cfg), is_ready_(false), stat_(stat)
        {}

        int send_request()
        {
            boost::property_tree::ptree pt;

            pt.put("cam.fps", stat_->cam_fps_);
            pt.put("proc.fps", stat_->process_fps_);
            pt.put("t.proc", stat_->f_process_time_);
            pt.put("t.send", stat_->f_send_time_);
            pt.put("sent.bytes", stat_->bytes_sent_);
            pt.put("sent.frames", stat_->frames_sent_);
            pt.put("fr.size", stat_->frame_size_);
            pt.put("rst.num", stat_->num_rst_);
            pt.put("ecc.coef", stat_->ecc_payload_coef_);
            pt.put("jpg.a.q", stat_->jpeg_auto_quality_);

            std::stringstream ss;
            boost::property_tree::write_json(ss, pt);
            return trans_->send(ss.str());
        }

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
                        ret = send_request();
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
        stat_data_t * stat_;

        cfg_subscribers subs_;
};

static int stop_flag = 0;

#ifdef __unix__
#include <signal.h>
extern "C" void quit_signal_handler(int signum)
{
    if (stop_flag != 0) exit(0); // just exit already
    stop_flag = 1;

    printf("Will quit at next camera frame (repeat to kill now)\n");

}
#endif

int main(int argc, char** argv)
{
    using namespace log4cplus;
    using namespace log4cplus::helpers;

    try
    {
        PropertyConfigurator::doConfigure("log4cplus.properties");
    }
    catch(const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        exit(0);
    }

#ifdef __unix__
    signal(SIGINT, quit_signal_handler);
#endif

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

    boost::shared_ptr<boost::property_tree::ptree> cfg(new boost::property_tree::ptree());

    stat_data_t stat_collect;

    cfg_sync_thread resync(stop_flag, cmd_url, cfg, &stat_collect);
    boost::thread cfg_thread(boost::ref(resync));

    while (!resync.ready())
    {
        LOG4CPLUS_INFO(Logger::getInstance("main"), "Config server is not ready. Retrying...");
        boost::this_thread::sleep_for(boost::chrono::seconds(1));
    }

    //    display(3, *cfg);

    int bch_n = cfg->get<int>("cfg.bch.n");
    int bch_t = cfg->get<int>("cfg.bch.t");
    int w = cfg->get<int>("cfg.img.width");
    int h = cfg->get<int>("cfg.img.height");

    bchwrapper bch(bch_n, bch_t);

    cv::Size isize(w,h);

    std::string dataurl("tcp://127.0.0.1:");
    dataurl += cfg->get<std::string>("cfg.dataport");

    boost::scoped_ptr<camera> cam(infile.size() == 0
            ? new camera(w, h)
            : new camera(infile, w, h));

    boost::shared_ptr<jpeg_builder> jb(new jpeg_builder());

    monitor_queue<camera_frame_t> mq(20);
    camera& c = *cam;


    frame_producer producer(c, mq, stop_flag, &stat_collect);
    frame_processor processor(isize, mq, stop_flag, dataurl, jb, &stat_collect, bch);

    // subscribe on updates
    resync.subscribe(jb.get());
    resync.subscribe(&producer);
    resync.subscribe(&processor);

    boost::thread tproducer(boost::ref(producer));
    boost::thread tprocess(boost::ref(processor));

    tprocess.join();
    tproducer.join();
    cfg_thread.join();

    LOG4CPLUS_INFO(Logger::getInstance("main"), "Capture server stopped...");

    return 0;
}



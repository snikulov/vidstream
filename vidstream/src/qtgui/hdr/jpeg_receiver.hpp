#ifndef JPEG_RECEIVER_HPP__
#define JPEG_RECEIVER_HPP__


#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

#include <opencv2/opencv.hpp>

#include <types.hpp>
#include <jpeg/jpeg_builder.hpp>
#include <jpeg/jpeg_transport.hpp>
#include <jpeg/jpeg_history.hpp>
#include <jpeg/jpeg_rcv_stm.hpp>
#include <transport/transport.hpp>

#include <ocv/ocv_output.hpp>
#include <corrupt/corrupt_intro.hpp>
#include <channel/in_channel.hpp>

#include <jpeg/jpeg_stream_parser.hpp>

using namespace vidstream;

int MyErrorHandler(int status, const char* func_name, const char* err_msg, const char* file_name, int line, void*)
{
    std::cerr << "Woohoo, my own custom error handler" << std::endl;
    return 0;
}

class jpeg_receiver : public cfg_notify
{
public:
    jpeg_receiver(bool& stop, const std::string& url
            , boost::shared_ptr<corrupt_intro> error
            , boost::shared_ptr<jpeg_builder> jb
            , boost::shared_ptr<bchwrapper> codec
            )
            : stop_(stop), url_(url), waiting_(false), err_(error), jb_(jb), codec_(codec)
    {
//      cv::namedWindow("received");
    }

    ~jpeg_receiver()
    {
    }

    void operator()()
    {

        cv::namedWindow("received");

        boost::shared_ptr<jpeg_history> history(new jpeg_history(jb_));

        input_.reset(new in_channel(url_, *codec_, *err_));

        jpeg_transport jt;
        const std::vector<unsigned char>& s_mark = jt.start_mark();
        const std::vector<unsigned char>& e_mark = jt.end_mark();

        jpeg_stream_parser jstp(s_mark);

        jpeg_rcv_stm stm(jb_, history, s_mark, e_mark);

        unsigned long img_count = 0;
        size_t rst_num = 0;

        boost::shared_ptr< std::vector< uint8_t > > indata;

        while(!stop_)
        {

            if (stop_) break;


            indata = input_->get(false);

            if (!indata)
            {
                jstp.parse();
            }
            else
            {
                jstp.parse(*indata);
            }

            while (jstp.num_jpegs())
            {
                jpeg_data_t jpeg = jb_->build_jpeg_from_rst(jstp.get_jpeg());
                if (jpeg)
                {
                    cv::Mat m = cv::imdecode(cv::Mat(*jpeg), cv::IMREAD_UNCHANGED);
                    if (!m.empty())
                    {
                        cv::imshow("received", m);
                        cv::waitKey(5);
                    }
                }
            }

            waiting_ = false;
        }
    }

    void stop()
    {
        input_.reset();
        stop_ = true;
    }

    void cfg_changed(const boost::property_tree::ptree& cfg)
    {
        int n = cfg.get<int>("cfg.bch.n");
        int t = cfg.get<int>("cfg.bch.t");

        codec_->change_params(n, t);

        jb_->cfg_changed(cfg);
        err_->cfg_changed(cfg);
    }

private:
    /* data */
    bool& stop_;
    std::string url_;
    bool waiting_;
    boost::shared_ptr<corrupt_intro> err_;
    boost::shared_ptr<jpeg_builder> jb_;

    boost::shared_ptr<bchwrapper> codec_;
    boost::shared_ptr<in_channel>         input_;
};

#endif


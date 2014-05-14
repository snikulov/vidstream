#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

#include <opencv2/opencv.hpp>

#include <types.hpp>
#include <jpeg_builder.hpp>
#include <transport.hpp>

#include <ocv_output.hpp>



using namespace vidstream;

class receiver
{
public:
    receiver(const std::string& url, ocv_output* win
#if defined(BUILD_FOR_LINUX)
                , boost::shared_ptr<ecc> bch
#endif
            )
        : url_(url), socket_(-1),
          endpoint_(-1), stop_(false), waiting_(false), win_(win)
#if defined(BUILD_FOR_LINUX)
          , ecc_(bch)
#endif
    {
    }
    ~receiver()
    {
        if (endpoint_ >= 0 && socket_ >= 0)
        {
            nn_shutdown(socket_, endpoint_);
            endpoint_ = -1;
        }

        if (socket_ >= 0)
        {
            nn_close(socket_);
            socket_ = -1;
        }
    }

    void operator()()
    {
        jpeg_builder jbuilder;
        boost::scoped_ptr<transport> rcv(
                new transport(TRANSPORT_RECEIVE, url_
#if defined(BUILD_FOR_LINUX)
                        , ecc_
#endif
                    )
                );

        jpeg_data_t rcv_buf(new std::vector<unsigned char>);
        const char * mstart = "jpegstart";
        const char * mend = "jpegend";
        unsigned long img_count = 0;
        while(!stop_)
        {
            waiting_ = true;
            std::vector<unsigned char> buf;
            rcv->receive(buf);
            waiting_ = false;

            if(buf.end() != std::search(buf.begin(), buf.end(), mstart, mstart+9))
            {
                // jpeg start
                rcv_buf.reset(new std::vector<unsigned char>());
//                std::cout << "started new image data..." << std::endl;
            }
            else if(buf.end() != std::search(buf.begin(), buf.end(), mend, mend+7))
            {
                // jpeg end
                // need to reassemble
                rcv_buf->push_back(0xff);
                rcv_buf->push_back(0xd9);
                img_count++;
                std::vector<size_t> rst_idxs;
                get_all_rst_blocks(*rcv_buf, rst_idxs);
                std::cout << "img_count = "<< img_count << " rst blocks =" << rst_idxs.size() << std::endl;

                jpeg_data_t jpg = jbuilder.build_jpeg_from_rst(rcv_buf);
//                jbuilder.write(jpg, img_count);
                cv::Mat m = cv::imdecode(cv::Mat(*jpg), 1);
                if (!m.empty())
                {
                    win_->show(m);
                }
                else
                {
                    std::cout << "m is empty" << std::endl;
                }
            }
            else
            {
                rcv_buf->insert(rcv_buf->end(), buf.begin(), buf.end());
            }

        }
    }

    void stop()
    {
        stop_ = true;
        win_->stop();
        if (waiting_)
        {
            nn_term();
        }
    }

private:
    /* data */
    std::string url_;
    int socket_;
    int endpoint_;
    bool stop_;
    bool waiting_;
    ocv_output* win_;
#if defined(BUILD_FOR_LINUX)
    boost::shared_ptr<ecc> ecc_;
#endif
};

int main(int argc, char** argv)
{
    const std::string url("tcp://127.0.0.1:9999");
    monitor_queue<cv::Mat> mq(10);
    ocv_output win("received", mq);

#if defined(BUILD_FOR_LINUX)
    boost::shared_ptr<ecc> bch_ecc(new ecc(5, 4)); // bm, bt
    receiver rcv(url, bch_ecc, &win);
#else
    receiver rcv(url, &win);
#endif

    boost::thread display(win);
    boost::thread t(rcv);

    display.join();
    t.join();

    return 0;
}



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

#include <nn.h>
// #include <pubsub.h>
#include <pipeline.h>

using namespace vidstream;

class receiver
{
public:
    receiver(const std::string& url)
        : url_(url), socket_(-1),
          endpoint_(-1), stop_(false), waiting_(false)
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

        socket_ = nn_socket (AF_SP, NN_PULL);
        if (socket_ < 0)
        {
            std::cout << "Error: " << nn_strerror(nn_errno()) << std::endl;
            return;
        }
//        endpoint_ = nn_connect(socket_, url_.c_str());
        endpoint_ = nn_bind(socket_, url_.c_str());

        if (endpoint_ < 0)
        {
            std::cout << "Error: " << nn_strerror(nn_errno()) << std::endl;
            return;
        }
        jpeg_data_t rcv_buf(new std::vector<unsigned char>);
        const char * mstart = "jpegstart";
        const char * mend = "jpegend";
        unsigned long img_count = 0;
        while(!stop_)
        {
            char *buf = NULL;

            waiting_ = true;
            int bytes = nn_recv(socket_, &buf, NN_MSG, 0);
            waiting_ = false;

            if(!memcmp(buf, mstart, 9))
            {
                // jpeg start
                rcv_buf.reset(new std::vector<unsigned char>());
//                std::cout << "started new image data..." << std::endl;
            }
            else if(!memcmp(buf, mend, 7))
            {
                // jpeg end
                // need to reassemble
                img_count++;
                std::vector<size_t> rst_idxs;
                get_all_rst_blocks(*rcv_buf, rst_idxs);
                std::cout << "img_count = "<< img_count << " rst blocks =" << rst_idxs.size() << std::endl;
                
                jpeg_data_t jpg = jbuilder.build_jpeg_from_rst(rcv_buf);
                cv::Mat m = cv::imdecode(*jpg, CV_LOAD_IMAGE_COLOR);
                cv::namedWindow( "img", CV_WINDOW_AUTOSIZE );
                cv::imshow("img", m);
            }
            else
            {
                rcv_buf->insert(rcv_buf->end(), buf, buf+bytes);
            }

//            std::cout << "received : " << bytes << " bytes" << std::endl;

            nn_freemsg(buf);
            buf = NULL;
        }
    }

    void stop()
    {
        stop_ = true;
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
};

int main(int argc, char** argv)
{
    const std::string url("tcp://127.0.0.1:9999");
    receiver rcv(url);

    boost::thread t(rcv);
    t.join();

    return 0;
}



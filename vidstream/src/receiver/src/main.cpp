#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

#include <nn.h>
#include <pubsub.h>

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
        socket_ = nn_socket (AF_SP, NN_SUB);
        if (socket_ < 0)
        {
            std::cout << "Error: " << nn_strerror(nn_errno()) << std::endl;
            return;
        }

        if (nn_setsockopt (socket_, NN_SUB, NN_SUB_SUBSCRIBE, "", 0) < 0)
        {
            std::cout << "Error: " << nn_strerror(nn_errno()) << std::endl;
            return;
        }

        endpoint_ = nn_connect(socket_, url_.c_str());
        if (endpoint_ < 0)
        {
            std::cout << "Error: " << nn_strerror(nn_errno()) << std::endl;
            return;
        }

        while(!stop_)
        {
            char *buf = NULL;

            waiting_ = true;
            int bytes = nn_recv(socket_, &buf, NN_MSG, 0);
            waiting_ = false;

            std::cout << "received : " << bytes << " bytes" << std::endl;

            nn_freemsg(buf);
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



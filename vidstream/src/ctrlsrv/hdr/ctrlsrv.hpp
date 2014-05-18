#ifndef CONTROL_SRV_HPP__
#define CONTROL_SRV_HPP__

#include <string>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/shared_ptr.hpp>

#include <nn.h>
#include <reqrep.h>

typedef boost::shared_ptr<boost::property_tree::ptree> cfg_ptr_t;

class ctrlsrv
{
public:
    ctrlsrv(cfg_ptr_t cfg, const std::string& url, bool& stop)
        : cfg_(cfg), url_(url), stop_(stop)
          , socket_(-1), session_(-1)
    {
    }
    ~ctrlsrv()
    {
        if(socket_ >= 0 && session_ >= 0)nn_shutdown(socket_, session_);
        if(socket_ >= 0) nn_close(socket_);
    }

    void operator()()
    {
        socket_ = nn_socket (AF_SP, NN_REP);
        session_ = nn_bind (socket_, url_.c_str());
        while(!stop_)
        {
            char *buf = NULL;
            int bytes = nn_recv(socket_, &buf, NN_MSG, 0);
            if (bytes >= 0 && !stop_)
            {
                // got command - don't care for now
                // later will use it for statistic
                std::ostringstream out;
                write_json(out, *cfg_, false);
                std::string data(out.str());
                bytes = nn_send(socket_, data.c_str(), data.size(), 0);
                if (bytes != data.size())
                {
                    std::cerr << "Error send config" << std::endl;
                }

                nn_freemsg (buf);
            }
            else
            {
                // error case
            }
        }
    }

    void stop()
    {
        stop_ = true;
    }

private:
    /* data */
    cfg_ptr_t cfg_;
    std::string url_;
    bool& stop_;
    int socket_;
    int session_;
};

#endif // CONTROL_SRV_HPP__




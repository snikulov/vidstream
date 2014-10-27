#include "channel_int.hpp"

#include <boost/make_shared.hpp>

#include <nn.h>
#include <pipeline.h>

channel::channel(const std::string& inurl, const std::string& outurl)
: inurl_(inurl), outurl_(outurl)
{
    is_running_ = true;
    wt_ = boost::thread(boost::bind(&channel::processor, this));
}

channel::~channel()
{
    is_running_ = false;
    incond_.notify_one();
    wt_.join();
}

void channel::processor()
{

    // TODO: more error handlers !!!

    int in_s  = nn_socket(AF_SP, NN_PULL);
    int out_s = nn_socket(AF_SP, NN_PUSH);

    int in_edp = nn_bind(in_s, inurl_.c_str());
    int out_edp = nn_connect(out_s, outurl_.c_str());
    
    nn_pollfd poll[2];

    poll[0].fd = in_s;
    poll[0].events = NN_POLLIN;
    poll[1].fd = out_s;
    poll[1].events = NN_POLLOUT;


    while (is_running_)
    {
        int rc = nn_poll(poll, 2, 2000);

        if (rc >= 0)
        {
            if (poll[0].revents & NN_POLLIN)
            {
                void *buf = NULL;
                int nbytes = nn_recv(in_s, &buf, NN_MSG, 0);

                if (nbytes < 0) 
                {
                    // error... currently skip for next try
                    // TODO: will re-create socket...
                }
                else 
                {
                    uint8_t * data_ptr = static_cast<uint8_t*>(buf);
                    boost::mutex::scoped_lock lock(inmx_);
                    indata_.insert(indata_.end(), data_ptr, data_ptr + nbytes);
                    nn_freemsg(buf);
                    incond_.notify_one();
                }

            }

            if (poll[1].revents & NN_POLLOUT)
            {
                boost::mutex::scoped_lock lock(outmx_);
                if (!outdata_.empty())
                {
                    std::vector<uint8_t>& data = *outdata_.front();
                    nn_send(out_s, &data[0], data.size(), 0);
                    outdata_.pop_front();
                }
            }
        }
        else
        {
        }
    }

    nn_close(out_s);
    nn_close(in_s);
}

void channel::put(const std::vector<uint8_t>& src)
{
    if (!src.empty())
    {
        boost::mutex::scoped_lock lock(outmx_);
        boost::shared_ptr< std::vector<uint8_t> > data_ptr( new std::vector<uint8_t>(src.begin(), src.end()) );
        outdata_.insert(outdata_.end(), data_ptr);
    }
}

boost::shared_ptr< std::vector<uint8_t> > channel::get()
{
    boost::shared_ptr< std::vector<uint8_t> > data;

    boost::mutex::scoped_lock lock(inmx_);
    if (indata_.empty())
    {
        incond_.wait(lock);
    }

    if (!indata_.empty())
    {
        data->insert(data->end(), indata_.begin(), indata_.end());
        indata_.clear();
    }

    return data;
}
#include "in_channel_int.hpp"

#include <iostream>
#include <nanopp/nn.hpp>
#include <boost/make_shared.hpp>

in_channel::in_channel(const std::string& url, bchwrapper& codec)
    : url_(url), codec_(codec), is_running_(false), is_connected_(false)
{
    is_running_ = true;
    wt_ = boost::thread(boost::bind(&in_channel::processor, this));
}

in_channel::~in_channel()
{
    is_running_ = false;
    incond_.notify_one();

    wt_.join();
}

void in_channel::processor()
{

    while (is_running_)
    {
        connect();

        if (is_connected_ && is_data_on_socket())
        {
            // read data to queue
            read_data();
        }
        else
        {
            boost::this_thread::sleep_for(boost::chrono::milliseconds(5));
        }
    }
}

void in_channel::connect()
{
    if (!is_connected_)
    {
        try
        {
            int ret = 0;
            sock_.reset(new nn::socket(AF_SP, NN_PULL));

            int opt = 150; // ms
            sock_->setsockopt(NN_SOL_SOCKET, NN_RCVTIMEO, &opt, sizeof (opt));

            ret = sock_->bind(url_.c_str());
            if (ret >= 0)
            {
                is_connected_ = true;
            }
            else
            {
                std::cerr << "[E] bind: " << ret << std::endl;
            }
        }
        catch (std::exception& e)
        {
            is_connected_ = false;
            std::cerr << "[E] bind: " << e.what() << std::endl;
        }
    }
}

bool in_channel::is_data_on_socket()
{
    bool ret_val = false;

    nn_pollfd poll[1] = {0};
    poll[0].fd = sock_->get_sock_val();
    poll[0].events = NN_POLLIN;

    int rc = nn_poll(poll, 1, 200);

    if (rc > 0 && poll[0].revents & NN_POLLIN)
    {
        ret_val = true;
    }
    else
    {
        std::cerr << "rc = " << rc << std::endl;
        if (rc < 0)
        {
            // socket error occurred
            is_connected_ = false;
            std::cerr << "[E] poll : " << nn_strerror(nn_errno()) << std::endl;
        }
        // else timeout gets trigged... repeat
    }
    return ret_val;
}

void in_channel::read_data()
{
    int bytes = 0;
    char * buf = NULL;

    bytes = sock_->recv(&buf, NN_MSG, 0);

    if (bytes > 0)
    {
        boost::shared_ptr<itpp::Channel_Code> codec = codec_.get();

        std::string rcv_data(buf + sizeof(buf[0]), buf + bytes - sizeof(buf[0]));
        itpp::bvec rcv_signal(rcv_data);
        uint8_t data = 0;
        if (codec)
        {
            itpp::bvec decoded;
            codec->decode(rcv_signal, decoded);
            data = static_cast<uint8_t>(itpp::bin2dec(decoded));
        }
        else
        {
            data = static_cast<uint8_t>(itpp::bin2dec(rcv_signal));
        }
        boost::mutex::scoped_lock lk(inmx_);
        indata_.push_back(data);
        incond_.notify_one();
#if 0
        if (codec && buf[0] == '[')
        {
            std::string rcv_string(buf + sizeof(buf[0]), buf + bytes - sizeof(buf[0]));
            itpp::bvec rcv_signal(rcv_string);

            itpp::bvec decoded;
            codec->decode(rcv_signal, decoded);
            uint8_t data = static_cast<uint8_t>(itpp::bin2dec(decoded));

            boost::mutex::scoped_lock lk(inmx_);
            indata_.push_back(data);
            incond_.notify_one();

        }
        else
        {
            boost::mutex::scoped_lock lk(inmx_);
            indata_.insert(indata_.end(), buf, buf + bytes);
            incond_.notify_one();
        }
#endif

    }
    else
    {
        // ???
        std::cerr << "[E] recv : " << bytes << " " << nn_strerror(nn_errno())<< std::endl;
        boost::this_thread::sleep_for(boost::chrono::microseconds(100));
    }

}

boost::shared_ptr< std::vector< uint8_t > > in_channel::get(bool wait)
{
    boost::shared_ptr< std::vector< uint8_t > > ret_val;
    boost::mutex::scoped_lock lk(inmx_);

    if (indata_.empty() && wait)
    {
        incond_.wait(lk);
    }

    if (!indata_.empty())
    {
        ret_val.reset(new std::vector<uint8_t>(indata_.begin(), indata_.end()));
        indata_.clear();
    }

    return ret_val;
}


#include "in_channel_int.hpp"

#include <iostream>
#include <nanopp/nn.hpp>
#include <boost/make_shared.hpp>

in_channel::in_channel(const std::string& url, bchwrapper& codec, corrupt_intro& err)
    : url_(url), codec_(codec), corruptor_(err), is_running_(false), is_connected_(false)
      , bytes_count_(0), log_(log4cplus::Logger::getInstance("input_channel"))
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
    // create local logger for thread
    log4cplus::Logger logger = log4cplus::Logger::getInstance("input_channel");

#if defined(CHANNEL_DEBUG)
    dbgfile_.open("in_channel_dbg.dat", std::ios::binary|std::ios::trunc );
#endif

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
            LOG4CPLUS_DEBUG(logger, "is_connected_=" << is_connected_);
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
                LOG4CPLUS_ERROR(log_, "bind: returned " << ret);
            }
        }
        catch (std::exception& e)
        {
            is_connected_ = false;
            LOG4CPLUS_ERROR(log_, "bind: " << e.what());
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
        if (rc < 0)
        {
            // socket error occurred
            is_connected_ = false;
            LOG4CPLUS_ERROR(log_, "poll : " << nn_strerror(nn_errno()));
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
        LOG4CPLUS_TRACE(log_, "read data: " << bytes << " bytes");
        bytes_count_ += bytes;

        boost::shared_ptr<itpp::Channel_Code> codec = codec_.get();

#if defined(CHANNEL_DEBUG)
        dbgfile_.write(buf, bytes);
#endif

//        std::string rcv_data(buf + sizeof(buf[0]), buf + bytes - sizeof(buf[0]));
        //int ri = reinterpret_cast<int>(*buf);
        unsigned int ri = 0;
        uint8_t one = buf[3];
        uint8_t two = buf[2];
        uint8_t three = buf[1];
        uint8_t four = buf[0];
        ri = ((one << 24) | (two << 16) | (three << 8) | four);
        itpp::bvec rcvsignal(itpp::dec2bin(32, int(ri)));

        // corrupt signal in channel
        itpp::bvec chsignal = corruptor_.corrupt(rcvsignal);

        uint8_t data = 0;
        if (codec)
        {
#if defined(CHANNEL_DEBUG)
            std::cerr << "[I] " << __FUNCTION__ << " decode data" << std::endl;
#endif
            itpp::bvec decoded;
            codec->decode(chsignal, decoded);
            int conv = itpp::bin2dec(decoded);
            conv &= 0xFF;
            data = static_cast<uint8_t>(conv);
        }
        else
        {
            int conv = itpp::bin2dec(chsignal);
            conv &= 0xFF;
            data = static_cast<uint8_t>(conv);
        }

#if defined(CHANNEL_DEBUG)
//        dbgfile_.write(reinterpret_cast<const char*>(&data), sizeof(data));
#endif
        boost::mutex::scoped_lock lk(inmx_);
        indata_.push_back(data);
        incond_.notify_one();
    }
    else
    {
        LOG4CPLUS_ERROR(log_, "rcv error: " << nn_strerror(nn_errno()));
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


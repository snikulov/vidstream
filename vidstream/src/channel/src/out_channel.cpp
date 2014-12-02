#include "out_channel_int.hpp"

#include <iostream>
#include <stdexcept>
#include <nanopp/nn.hpp>
#include <boost/make_shared.hpp>
#include <utils/converters.hpp>

out_channel::out_channel(const std::string& url, bchwrapper& codec, stat_data_t* stat)
    : url_(url), codec_(codec), stat_(stat), is_running_(false), is_connected_(false)
      , block_count_(0), bytes_count_(0), log_(log4cplus::Logger::getInstance("output_channel"))
{
    is_running_ = true;
    wt_ = boost::thread(boost::bind(&out_channel::processor, this));
}

out_channel::~out_channel()
{
    is_running_ = false;
    outcond_.notify_one();

    wt_.join();
}

bool out_channel::can_send_data()
{
    bool ret_val = false;

    nn_pollfd poll[1] = { 0 };
    poll[0].fd = sock_->get_sock_val();
    poll[0].events = NN_POLLOUT;

    int rc = nn_poll(poll, 1, 200);

    if (rc > 0 && poll[0].revents & NN_POLLOUT)
    {
        ret_val = true;
    }
    else
    {
#if defined(CHANNEL_DEBUG)
        std::cerr << "rc = " << rc << std::endl;
#endif
        if (rc < 0)
        {
            // socket error occurred
            is_connected_ = false;
#if defined(CHANNEL_DEBUG)
            std::cerr << "[E] can_send_data poll : " << nn_strerror(nn_errno()) << std::endl;
#endif
        }
        // else timeout gets trigged... repeat
    }
    return ret_val;
}

void out_channel::connect()
{
    if (!is_connected_)
    {
        try
        {
            sock_.reset(new nn::socket(AF_SP, NN_PUSH));

            int opt = 150;
            sock_->setsockopt(NN_SOL_SOCKET, NN_SNDTIMEO, &opt, sizeof (opt));
            if (sock_->connect(url_.c_str()) >= 0)
            {
                is_connected_ = true;
            }
            else
            {
#if defined(CHANNEL_DEBUG)
                std::cerr << "[E] connect: " << nn_strerror(nn_errno()) << std::endl;
#endif
            }
        }
        catch (std::exception& e)
        {
            is_connected_ = false;
#if defined(CHANNEL_DEBUG)
            std::cerr << "[E] connect: " << e.what() << std::endl;
#endif
        }
    }
}

int out_channel::send_encoded(const std::vector<uint8_t>& data, boost::shared_ptr<abstract_ecc_codec> codec)
{
    std::vector<uint8_t> dsend;

    size_t len = data.size();
    size_t res = 0;

    codec->encode(data, dsend);

    LOG4CPLUS_DEBUG(log_, "data.size() = " << data.size() << " encoded.size() = " << dsend.size());

    int sent = sock_->send(reinterpret_cast<const char*>(&dsend[0]), dsend.size(), 0);

    if (sent > 0)
    {
        res += sent;
        bytes_count_ +=sent;
    }
    else
    {
        LOG4CPLUS_WARN(log_, "sent = " << sent << " " << nn_strerror(nn_errno()));
        while (is_running_ && !can_send_data())
        {
            // wait for unblocking...
        }
        sent = sock_->send(reinterpret_cast<const char*>(&dsend[0]), dsend.size(), 0);
        if (sent > 0)
        {
            res += sent;
            bytes_count_ += sent;
        }
        else
        {
            LOG4CPLUS_FATAL(log_, "can't send - sent = " << sent << " " << nn_strerror(nn_errno()));
            throw std::runtime_error("can't send");
        }
    }

    stat_->bytes_sent_ = bytes_count_/stat_->tsec_;

    return static_cast<int>(res);
}

boost::shared_ptr< std::vector<uint8_t> > out_channel::getdata()
{

    boost::mutex::scoped_lock lk(outmx_);

    if (outdata_.empty())
    {
        outcond_.wait(lk);
    }

    boost::shared_ptr< std::vector<uint8_t> > data_ptr;
    if (!outdata_.empty())
    {
        data_ptr = outdata_.front();
    }
    return data_ptr;
}

void out_channel::send_data()
{
    bool empty = true;
    boost::shared_ptr< std::vector<uint8_t> > data_ptr = getdata();

    if (data_ptr)
    {
        std::vector<uint8_t>& data = *data_ptr;
        size_t len = data.size();
        int sent = 0;

        sent = send_encoded(data, codec_.get());

        if (sent > 0)
        {
            // remove from queue
            boost::mutex::scoped_lock lk(outmx_);
            outdata_.pop_front();
            block_count_++;
        }
        else
        {
            // sleep or poll???
            LOG4CPLUS_DEBUG(log_, "send_encoded returned " << sent);
        }
    }
    stat_->frames_sent_ = block_count_;
}

void out_channel::processor()
{
#if defined(CHANNEL_DEBUG)
    dbgfile_.open("out_channel_dbg.dat", std::ios::binary|std::ios::trunc );
#endif

    while (is_running_)
    {
        connect();

        if (is_connected_)
        {
            send_data();
        }
        else
        {
            boost::this_thread::sleep_for(boost::chrono::milliseconds(5));
        }
    }
#if defined(CHANNEL_DEBUG)
    dbgfile_.close();
#endif
}

void out_channel::put(boost::shared_ptr< std::vector<uint8_t> > data)
{
    boost::mutex::scoped_lock lock(outmx_);
    if (outdata_.size() > 20)
    {
        LOG4CPLUS_WARN(log_, "Output queue is full, frame is dropped!");
    }
    else
    {
        outdata_.push_back(data);
    }
    outcond_.notify_one();
}

void out_channel::put(const std::vector<uint8_t>& data)
{
    boost::shared_ptr< std::vector<uint8_t> > buf(new std::vector<uint8_t>(data.begin(), data.end()));
    put(buf);
}

void out_channel::put(const uint8_t * data, size_t len)
{
    std::vector<uint8_t> buf(data, data + len);
    put(buf);
}

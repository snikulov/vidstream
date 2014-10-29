#include "out_channel_int.hpp"

#include <iostream>
#include <nanopp/nn.hpp>
#include <boost/make_shared.hpp>

out_channel::out_channel(const std::string& url, boost::shared_ptr<itpp::Channel_Code> codec)
    : url_(url), codec_(codec), is_running_(false), sock_(AF_SP, NN_PUSH), is_connected_(false)
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

void out_channel::connect()
{
    if (!is_connected_)
    {
        try
        {
            sock_.connect(url_.c_str());
            is_connected_ = true;
        }
        catch (std::exception& e)
        {
            is_connected_ = false;
            std::cerr << "[E] connect: " << e.what() << std::endl;
        }
    }
}

int out_channel::send_encoded(const std::vector<uint8_t>& data)
{
    size_t len = data.size();
    size_t res = 0;
    for (size_t i = 0; i < len; i++)
    {
        itpp::bvec bits = itpp::dec2bin(data[i]);
        itpp::bvec encoded = codec_->encode(bits);
        std::string str = itpp::to_str(encoded);
        size_t strl = str.size();
        int sent = sock_.send(str.c_str(), strl, NN_DONTWAIT);
        if (sent >= 0 && sent == strl)
        {
            res += sent;
        }
        else
        {
            return sent;
        }
    }
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
        if (codec_)
        {
            sent = send_encoded(data);
        }
        else
        {
            sent = sock_.send(reinterpret_cast<const char*>(&data[0]), data.size(), NN_DONTWAIT);
        }

        if (sent >= 0 && sent == len)
        {
            // remove from queue
            boost::mutex::scoped_lock lk(outmx_);
            outdata_.pop_front();
        }
        else
        {
            // reconnect?
            // now - will try again
        }
    }


}

void out_channel::processor()
{

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
}

void out_channel::put(boost::shared_ptr< std::vector<uint8_t> > data)
{
    boost::mutex::scoped_lock lock(outmx_);
    outdata_.push_back(data);
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
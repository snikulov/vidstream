#include "out_channel_int.hpp"

#include <iostream>
#include <nanopp/nn.hpp>
#include <boost/make_shared.hpp>

out_channel::out_channel(const std::string& url, bchwrapper& codec)
    : url_(url), codec_(codec), is_running_(false), is_connected_(false)
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
        std::cerr << "rc = " << rc << std::endl;
        if (rc < 0)
        {
            // socket error occurred
            is_connected_ = false;
            std::cerr << "[E] can_send_data poll : " << nn_strerror(nn_errno()) << std::endl;
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
                std::cerr << "[E] connect: " << nn_strerror(nn_errno()) << std::endl;
            }
        }
        catch (std::exception& e)
        {
            is_connected_ = false;
            std::cerr << "[E] connect: " << e.what() << std::endl;
        }
    }
}

int out_channel::send_encoded(const std::vector<uint8_t>& data, boost::shared_ptr<itpp::Channel_Code> codec)
{
    size_t len = data.size();

    size_t res = 0;
    for (size_t i = 0; i < len; ++i)
    {
        itpp::bvec bits = itpp::dec2bin(data[i]);
        itpp::bvec encoded = codec->encode(bits);
        std::string str = itpp::to_str(encoded);
        size_t strl = str.size();
        int sent = sock_->send(str.c_str(), strl, 0);
        if (sent > 0 && sent == strl)
        {
            res += sent;
        }
        else
        {

            std::cerr << "[E] send_encoded: sent=" << sent << " "
                << nn_strerror(nn_errno()) << std::endl;

            while (is_running_ && !can_send_data())
            {
                // wait for unblocking...
            }
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
        boost::shared_ptr<itpp::Channel_Code> codec = codec_.get();
        if (codec)
        {
//            std::cerr << "[I] send encoded with BCH" << std::endl;
            sent = send_encoded(data, codec);
        }
        else
        {
//            std::cerr << "[I] send without encoding" << std::endl;
            sent = sock_->send(reinterpret_cast<const char*>(&data[0]), data.size(), 0);
        }

        if (sent > 0)
        {
            // remove from queue
            boost::mutex::scoped_lock lk(outmx_);
            outdata_.pop_front();
        }
        else
        {
            std::cerr << "[I] sent = " << sent << " len = " << len << std::endl;
            // sleep or poll???
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
    if (outdata_.size() > 20)
    {
        std::cerr << "[W] more then limit in output queue - dropping frame" << std::endl;
        return;
    }
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

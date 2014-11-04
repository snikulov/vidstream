#ifndef IN_CHANNEL_INT_HPP__

#define IN_CHANNEL_INT_HPP__

#include <vector>
#include <deque>

#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/atomic.hpp>

#include <nanopp/nn.hpp>
#include <itpp/itcomm.h>

class in_channel
    : private boost::noncopyable
{
public:
    in_channel(const std::string& url, boost::shared_ptr<itpp::Channel_Code> codec);
    ~in_channel();

    // blocks on wait if no data
    boost::shared_ptr< std::vector< uint8_t > > get(bool wait=true);

    void in_channel::set_codec(boost::shared_ptr<itpp::Channel_Code> codec)
    {
        boost::mutex::scoped_lock lk(codec_lk_);
        codec_ = codec;
    }

private:
    in_channel();

    void processor();

    void connect();



    bool is_data_on_socket();

    void read_data();

    int send_encoded(const std::vector<uint8_t>& data);

    boost::shared_ptr< std::vector<uint8_t> > getdata();

    std::string url_;

    boost::mutex codec_lk_;
    boost::shared_ptr<itpp::Channel_Code> codec_;

    // internal cbuff
    boost::mutex inmx_;
    boost::condition_variable incond_;
    std::deque< uint8_t > indata_;

    bool is_running_;
    boost::shared_ptr<nn::socket> sock_;
    bool is_connected_;

    boost::thread wt_;
};


#endif /* end of include guard: IN_CHANNEL_INT_HPP__ */

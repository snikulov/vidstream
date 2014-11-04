#ifndef OUT_CHANNEL_INT_HPP__

#define OUT_CHANNEL_INT_HPP__

#include <vector>
#include <deque>
#include <fstream>

#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <nanopp/nn.hpp>
#include <itpp/itcomm.h>
#include <channel/bchwrapper.hpp>

class out_channel
    : private boost::noncopyable
{
public:
    out_channel(const std::string& url, bchwrapper& codec);
    ~out_channel();

    void put(boost::shared_ptr< std::vector<uint8_t> > data);
    void put(const std::vector<uint8_t>& data);
    void put(const uint8_t* data, size_t len);

private:
    out_channel();

    void processor();

    void connect();

    void send_data();

    int send_encoded(const std::vector<uint8_t>& data, boost::shared_ptr<itpp::Channel_Code> codec);

    boost::shared_ptr< std::vector<uint8_t> > getdata();

    bool can_send_data();

    std::string url_;
    bchwrapper& codec_;

    // internal cbuff
    boost::mutex outmx_;
    boost::condition_variable outcond_;
    std::deque< boost::shared_ptr< std::vector<uint8_t> > > outdata_;

    bool is_running_;
    boost::shared_ptr<nn::socket> sock_;
    bool is_connected_;

    boost::thread wt_;

#if defined(CHANNEL_DEBUG)
    std::ofstream dbgfile_;
#endif
};


#endif /* end of include guard: OUT_CHANNEL_INT_HPP__ */

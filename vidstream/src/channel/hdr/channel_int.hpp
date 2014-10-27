#ifndef CHANNEL_INT_HPP__
#define CHANNEL_INT_HPP__

#include <deque>
#include <vector>

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <itpp/itcomm.h>

class channel : private boost::noncopyable
{
public:
    channel(const std::string& inurl, const std::string& outurl);
    ~channel();

    void put(const std::vector<uint8_t>& src);
    boost::shared_ptr< std::vector<uint8_t> > get(bool wait = true);

    void set_codec(boost::shared_ptr< itpp::Channel_Code > codec);

    bool is_data_ready();

    size_t in_data_size();
    
private:
    channel();

    void send_encoded(int s, const std::vector<uint8_t>& src);

    void processor();

    bool is_running_;

    std::string inurl_;
    std::string outurl_;

    // input data
    boost::mutex inmx_;
    boost::condition_variable incond_;
    std::deque< uint8_t > indata_;

    // output data
    boost::mutex outmx_;
    boost::condition_variable outcond_;
    std::deque< boost::shared_ptr< std::vector<uint8_t> > > outdata_;

    boost::thread wt_;

    boost::shared_ptr< itpp::Channel_Code > codec_;

};

#endif



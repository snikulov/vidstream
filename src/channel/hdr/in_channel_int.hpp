#ifndef IN_CHANNEL_INT_HPP__

#define IN_CHANNEL_INT_HPP__

#include <vector>
#include <deque>
#include <fstream>

#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/atomic.hpp>

#include <nanopp/nn.hpp>

#include <channel/bchwrapper.hpp>
#include <corrupt/corrupt_intro.hpp>
#include <perf/perf_clock.hpp>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/loglevel.h>

class in_channel
    : private boost::noncopyable
{
public:
    in_channel(const std::string& url, bchwrapper& codec, corrupt_intro& err);
    ~in_channel();

    // blocks on wait if no data
    boost::shared_ptr< std::vector< uint8_t > > get(bool wait=true);

private:
    in_channel();

    void processor();

    void connect();



    bool is_data_on_socket();

    void read_data();

    boost::shared_ptr< std::vector<uint8_t> > getdata();

    std::string url_;

    boost::mutex codec_lk_;
    bchwrapper& codec_;

    corrupt_intro& corruptor_;

    // internal cbuff
    boost::mutex inmx_;
    boost::condition_variable incond_;
    std::deque< uint8_t > indata_;

    bool is_running_;
    boost::shared_ptr<nn::socket> sock_;
    bool is_connected_;

    unsigned long long bytes_count_;
    timer<boost::chrono::steady_clock> timer_;

    boost::thread wt_;
#if defined(CHANNEL_DEBUG)
    std::ofstream dbgfile_;
#endif
    log4cplus::Logger log_;
};


#endif /* end of include guard: IN_CHANNEL_INT_HPP__ */

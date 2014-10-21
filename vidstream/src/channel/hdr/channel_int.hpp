#ifndef CHANNEL_INT_HPP__
#define CHANNEL_INT_HPP__

#include <deque>
#include <vector>

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

class channel : private boost::noncopyable
{
public:
    channel();
    ~channel();

    void put(const std::vector<uint8_t>& src);
    void get(std::vector<uint8_t>& dst);
    
private:

    void worker();
    bool is_running_;

    boost::mutex mx_;
    boost::condition_variable cond_;
    std::deque<uint8_t> data_;

    boost::thread wt_;

};

#endif



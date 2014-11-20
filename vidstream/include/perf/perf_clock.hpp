#ifndef PERF_CLOCK_HPP__
#define PERF_CLOCK_HPP__

#include <boost/chrono.hpp>
using namespace boost::chrono;

template< class Clock >
class timer
{
    typename Clock::time_point start_;
    mutable typename Clock::duration diff_;

public:
    timer() : start_( Clock::now() ) {}

    void start(bool reset = true)
    {
        if (reset)
        {
            start_ = Clock::now();
        }
    }

    void stop()
    {
        diff_ = Clock::now() - start_;
    }

    typename Clock::duration elapsed() const
    {
        return diff_;
    }

#if 0
    double seconds() const
    {
        return diff_.count() * ((double)Clock::period::num/Clock::period::den);
    }
#endif

    unsigned long long sec() const
    {
        return  (boost::chrono::duration_cast<boost::chrono::seconds>( diff_ )).count();
    }

    unsigned long long nsec() const
    {
        return diff_.count();
    }

    unsigned long long mcsec() const
    {
        return  boost::chrono::duration_cast<boost::chrono::microseconds>( elapsed() ).count();
    }
};


#endif


#ifndef PERF_CLOCK_HPP__
#define PERF_CLOCK_HPP__

#include <boost/chrono.hpp>
using namespace boost::chrono;

template< class Clock >
class timer
{
    typename Clock::time_point start_;

public:
    timer() : start_( Clock::now() ) {}

    void restart()
    {
        start_ = Clock::now();
    }

    typename Clock::duration elapsed() const
    {
        return Clock::now() - start_;
    }

    unsigned long long sec() const
    {
        return  boost::chrono::duration_cast<boost::chrono::seconds>( elapsed() ).count();
    }

    unsigned long long nsec() const
    {
        return elapsed().count();
    }

    unsigned long long mcsec() const
    {
        return  boost::chrono::duration_cast<boost::chrono::microseconds>( elapsed() ).count();
    }

    unsigned long long sec(typename Clock::duration& d) const
    {
        return  boost::chrono::duration_cast<boost::chrono::seconds>( d ).count();
    }

    unsigned long long nsec(typename Clock::duration& d) const
    {
        return d.count();
    }

    unsigned long long mcsec(typename Clock::duration& d) const
    {
        return boost::chrono::duration_cast<boost::chrono::microseconds>( d ).count();
    }

};


#endif


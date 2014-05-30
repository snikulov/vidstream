#ifndef PERF_CLOCK_HPP__
#define PERF_CLOCK_HPP__

#include <boost/chrono.hpp>
using namespace boost::chrono;

template< class Clock >
class timer
{
  typename Clock::time_point start;
public:
  timer() : start( Clock::now() ) {}
  typename Clock::duration elapsed() const
  {
    return Clock::now() - start;
  }
  double seconds() const
  {
    return elapsed().count() * ((double)Clock::period::num/Clock::period::den);
  }
};


#endif 


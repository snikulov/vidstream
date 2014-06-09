#define BOOST_TEST_MODULE test_perf
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include <boost/thread.hpp>
#include <perf/perf_clock.hpp>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_suite_perf)

BOOST_AUTO_TEST_CASE( test_perf_case_1 )
{
    timer<high_resolution_clock> t;
    boost::this_thread::sleep_for(boost::chrono::seconds(1));
    t.stop();
    BOOST_MESSAGE("sleeped for: nsec=" << t.nsec() << " int. sec=" << t.sec() <<" real sec=" << t.seconds());

    t.start();
    for(int i=0; i < 5; ++i)
    {
        boost::this_thread::sleep_for(boost::chrono::seconds(1));
    }
    t.stop();
    BOOST_CHECK_MESSAGE(5 == t.sec(), "timer sec=" << t.sec());

}

BOOST_AUTO_TEST_SUITE_END()




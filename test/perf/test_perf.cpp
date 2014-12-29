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
    BOOST_MESSAGE("sleeped for: nsec=" << t.nsec() << " int. sec=" << t.sec() <<" real sec=" << t.sec());

    t.restart();
    for(int i=0; i < 5; ++i)
    {
        boost::this_thread::sleep_for(boost::chrono::seconds(1));
    }
    BOOST_CHECK_MESSAGE(5 == t.sec(), "timer sec=" << t.sec());

}

BOOST_AUTO_TEST_CASE( test_perf_case_2 )
{
    boost::chrono::duration<unsigned int, boost::ratio<1, 25> > time_per_frame(1);
    boost::chrono::nanoseconds ns = boost::chrono::duration_cast<boost::chrono::nanoseconds>(time_per_frame);
    boost::chrono::milliseconds ms = boost::chrono::duration_cast<boost::chrono::milliseconds>(time_per_frame);
    BOOST_MESSAGE("ns = " << ns.count() << " ms = " << ms.count());
}


BOOST_AUTO_TEST_CASE( test_perf_case_3 )
{

    unsigned long b = 10000000;
    BOOST_CHECK((b/8) == 1250000);
}



BOOST_AUTO_TEST_SUITE_END()




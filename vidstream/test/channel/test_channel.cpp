#define BOOST_TEST_MODULE test_channel
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include "channel/channel.hpp"

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_suite_channel)

BOOST_AUTO_TEST_CASE( test_channel_case_1 )
{
    channel ch("tcp://127.0.0.1:9000", "tcp://127.0.0.1:9000");
    boost::this_thread::sleep_for(boost::chrono::microseconds(100));
    BOOST_MESSAGE("test case with channel");

    std::vector<uint8_t> src;
    boost::shared_ptr< std::vector<uint8_t> > dst;
    uint8_t s = 0xFF;
    for (size_t i = 0; i < 20000; ++i)
    {
        src.push_back(s);
        ch.put(src);
        dst = ch.get();
        BOOST_REQUIRE(dst);
        BOOST_REQUIRE(src == *dst);
    }
}



BOOST_AUTO_TEST_SUITE_END()




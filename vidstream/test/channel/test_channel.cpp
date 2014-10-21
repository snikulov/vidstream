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
    channel ch;
    boost::this_thread::sleep_for(boost::chrono::microseconds(100));
    BOOST_MESSAGE("test case with channel");

    std::vector<uint8_t> src;
    src.push_back(8);
    std::vector<uint8_t> dst;

    ch.put(src);
    ch.get(dst);

    BOOST_REQUIRE(src == dst);
}



BOOST_AUTO_TEST_SUITE_END()




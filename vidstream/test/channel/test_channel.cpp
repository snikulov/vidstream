#define BOOST_TEST_MODULE test_channel
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include "channel/channel.hpp"

#include "channel/out_channel.hpp"
#include "channel/in_channel.hpp"

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
    for (size_t i = 0; i < 200; ++i)
    {
        src.push_back(s);
        ch.put(src);
        dst = ch.get();
        BOOST_REQUIRE(dst);
        BOOST_REQUIRE(src == *dst);
    }
}

BOOST_AUTO_TEST_CASE(test_channel_case_2)
{
    channel ch1("tcp://127.0.0.1:9000", "tcp://127.0.0.1:9001");
    channel ch2("tcp://127.0.0.1:9001", "tcp://127.0.0.1:9000");

    boost::this_thread::sleep_for(boost::chrono::microseconds(200));
    BOOST_MESSAGE("test case with two channel");

    std::vector<uint8_t> src;
    boost::shared_ptr< std::vector<uint8_t> > dst1;
    boost::shared_ptr< std::vector<uint8_t> > dst2;
    uint8_t s = 0xFF;

    for (size_t i = 0; i < 200; ++i)
    {
        src.push_back(s);
        ch1.put(src);
        ch2.put(src);

        dst1 = ch1.get();
        dst2 = ch2.get();

        BOOST_REQUIRE(dst1);
        BOOST_REQUIRE(dst2);
        BOOST_REQUIRE(src == *dst1);
        BOOST_REQUIRE(src == *dst2);
    }
}

BOOST_AUTO_TEST_CASE(test_channel_case_3)
{
    channel ch("tcp://127.0.0.1:9000", "tcp://127.0.0.1:9000");
    boost::this_thread::sleep_for(boost::chrono::microseconds(100));
    BOOST_MESSAGE("test case with channel");

    boost::shared_ptr<itpp::Channel_Code> codec(new itpp::BCH(7, 3));
    ch.set_codec(codec);

    
    boost::shared_ptr< std::vector<uint8_t> > dst;
    uint8_t s = 0xFF;
    for (size_t i = 0; i < s; ++i)
    {
        std::vector<uint8_t> src;
        src.push_back(i);
        ch.put(src);
        dst = ch.get();
        BOOST_REQUIRE(dst);
        BOOST_REQUIRE(src == *dst);
        BOOST_REQUIRE(i == dst->at(0));
    }
}

BOOST_AUTO_TEST_CASE(test_channel_case_4)
{
    channel ch("tcp://127.0.0.1:9000", "tcp://127.0.0.1:9000");
    boost::this_thread::sleep_for(boost::chrono::microseconds(100));
    BOOST_MESSAGE("test case with channel");

    boost::shared_ptr<itpp::Channel_Code> codec(new itpp::BCH(7, 3));
    ch.set_codec(codec);


    boost::shared_ptr< std::vector<uint8_t> > dst;
    std::vector<uint8_t> src;
    uint8_t s = 0xFF;
    for (size_t i = 0; i < s; ++i)
    {
        src.push_back(i);
        ch.put(src);
        while (ch.in_data_size() != src.size())
        {
            // sleep
            // boost::this_thread::sleep_for(boost::chrono::nanoseconds(5));
        }
        dst = ch.get();
        BOOST_REQUIRE(dst);
        BOOST_REQUIRE(src == *dst);
    }
}


BOOST_AUTO_TEST_CASE(test_channel_case_5)
{
    boost::shared_ptr<itpp::Channel_Code> bch_codec(new itpp::BCH(7, 3));
    boost::shared_ptr<itpp::Channel_Code> empty_codec;

    in_channel in_plain("tcp://127.0.0.1:9000", empty_codec);
    boost::this_thread::sleep_for(boost::chrono::milliseconds(100));

    out_channel out_plain("tcp://127.0.0.1:9000", empty_codec);


    std::vector<uint8_t> test1(20, 0xff);
    out_plain.put(test1);

    boost::shared_ptr< std::vector<uint8_t> > received = in_plain.get();

    BOOST_REQUIRE(test1 == *received);

}

BOOST_AUTO_TEST_SUITE_END()




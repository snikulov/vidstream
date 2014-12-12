#define BOOST_TEST_MODULE test_channel
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include <opencv2/opencv.hpp>
#include <types.hpp>

#include "channel/channel.hpp"
#include "channel/out_channel.hpp"
#include "channel/in_channel.hpp"
#include "channel/bchwrapper.hpp"


#include <jpeg/jpeg_rcv_stm.hpp>
#include <jpeg/jpeg_transport.hpp>
#include <jpeg/jpeg_history.hpp>
#include <jpeg/jpeg_builder.hpp>

#include <jpeg/jpeg_stream_parser.hpp>

#include <corrupt/corrupt_intro.hpp>
#include <stat/stat_data.hpp>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/loglevel.h>
#include <log4cplus/configurator.h>

using namespace boost::unit_test;

struct fixture
{
    fixture()
    {
        log4cplus::BasicConfigurator::doConfigure();
        log_ = log4cplus::Logger::getInstance("test");
    }
    ~fixture() {}

    log4cplus::Logger log_;
};

BOOST_FIXTURE_TEST_SUITE(test_suite_channel, fixture)

#if 0
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
    boost::shared_ptr<itpp::Channel_Code> empty_codec;

    in_channel in_plain("tcp://127.0.0.1:9000", empty_codec);

    // setle the server connect
    // TODO: need fix it later
    boost::this_thread::sleep_for(boost::chrono::seconds(1));

    out_channel out_plain("tcp://127.0.0.1:9000", empty_codec);

    std::vector<uint8_t> test1(20, 0xff);
    out_plain.put(test1);

    boost::shared_ptr< std::vector<uint8_t> > received = in_plain.get();

    BOOST_REQUIRE(test1 == *received);

}

BOOST_AUTO_TEST_CASE(test_channel_case_5)
{
    boost::shared_ptr<itpp::Channel_Code> bch_codec(new itpp::BCH(7, 3));

    in_channel in_plain("tcp://127.0.0.1:9000", bch_codec);

    // settle the server connect
    // TODO: need fix it later
    boost::this_thread::sleep_for(boost::chrono::seconds(1));

    stat_data_t stat;
    out_channel out_plain("tcp://127.0.0.1:9000", bch_codec, &stat);

    std::vector<uint8_t> test1(20, 0xff);
    out_plain.put(test1);

    std::vector<uint8_t> target;
    while(target.size() != test1.size())
    {

        boost::shared_ptr< std::vector<uint8_t> > received = in_plain.get();
        target.insert(target.end(), received->begin(), received->end());
    }

    BOOST_CHECK_MESSAGE(test1 == target,
            "test1.size()=" << test1.size() << " received.size()=" << target.size());
}
#endif


using namespace vidstream;

BOOST_AUTO_TEST_CASE(test_channel_case_5)
{
    itpp::bvec t = itpp::dec2bin(32, int(0xf0fffc07));
}

BOOST_AUTO_TEST_CASE(test_channel_case_6)
{
    int i = 0;
    // use jpeg for test
    BOOST_REQUIRE(framework::master_test_suite().argc > 1);

    corrupt_intro corrupt;
    bchwrapper bch_codec(5, 3);

    in_channel in_plain("tcp://127.0.0.1:9000", bch_codec, corrupt);

    // settle the server connect
    // TODO: need fix it later
    boost::this_thread::sleep_for(boost::chrono::seconds(1));
    stat_data_t stat;
    boost::shared_ptr<out_channel> out_plain(new out_channel("tcp://127.0.0.1:9000"
                , bch_codec, &stat));

    jpeg_data_t data = jpeg_builder::read(framework::master_test_suite().argv[1]);

    BOOST_REQUIRE(data);
    BOOST_REQUIRE(!data->empty());

    // jpeg header check
    std::vector<size_t> outidx;
    BOOST_REQUIRE(get_all_rst_blocks(*data, outidx));
    BOOST_REQUIRE(!outidx.empty());
    BOOST_TEST_MESSAGE("rst_count: " << outidx.size());


    boost::shared_ptr<jpeg_builder> jb_(new jpeg_builder());

    jpeg_rst_idxs_t rst(jb_->rst_idxs(data));

    boost::shared_ptr<jpeg_history> history;
    jpeg_transport jt;
    const std::vector<unsigned char>& s_mark = jt.start_mark();

    jpeg_stream_parser stream_parser(jt.start_mark());

    for (size_t idx = 0; idx < 5; ++idx)
    {
        size_t ret = jt.send_jpeg(data, rst, out_plain);

        while (stream_parser.num_jpegs() != 1)
        {
            boost::shared_ptr<std::vector< uint8_t > > buf = in_plain.get(false);
            if (buf)
            {
                stream_parser.parse(*buf);
            }
            else
            {
                stream_parser.parse();
                boost::this_thread::sleep_for(boost::chrono::nanoseconds(200));
            }
        }

        jb_->write(jb_->build_jpeg_from_rst(stream_parser.get_jpeg()), idx);
    }
}

BOOST_AUTO_TEST_CASE(test_channel_case_7)
{
    int i = 0;
    // use jpeg for test
    BOOST_REQUIRE(framework::master_test_suite().argc > 1);

    corrupt_intro corrupt(0.1);
    bchwrapper bch_codec(5, 3);
    in_channel in_plain("tcp://127.0.0.1:9000", bch_codec, corrupt);

    // settle the server connect
    // TODO: need fix it later
    boost::this_thread::sleep_for(boost::chrono::seconds(1));
    stat_data_t stat;
    boost::shared_ptr<out_channel> out_plain(new out_channel("tcp://127.0.0.1:9000"
                , bch_codec, &stat));

    jpeg_data_t data = jpeg_builder::read(framework::master_test_suite().argv[1]);

    BOOST_REQUIRE(data);
    BOOST_REQUIRE(!data->empty());

    std::vector<uint8_t>& data_ref = *data;
    std::vector<uint8_t> encoded_data;

    bch_codec.get()->encode(data_ref, encoded_data);

    std::vector<uint8_t> signal = corrupt.corrupt(encoded_data);

    std::vector<uint8_t> decoded_data;

    bch_codec.get()->decode(signal, decoded_data);

    BOOST_CHECK(data_ref == decoded_data);
}


BOOST_AUTO_TEST_SUITE_END()




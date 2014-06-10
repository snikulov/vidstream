#define BOOST_TEST_MODULE test_corrupt
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include <corrupt/corrupt.h>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_suite_corrupt)

BOOST_AUTO_TEST_CASE( test_corrupt_case_1 )
{
    std::vector<uint8_t> data(200, 1);
    std::vector<uint8_t> etalon(200, 1);
    corruptor corr;

    BOOST_CHECK(etalon == data);

    double err_persent = 1.0;
    corr.add_error(data, err_persent);

    BOOST_CHECK(etalon != data);
    double err = corr.check_error(etalon, data);

    BOOST_CHECK_MESSAGE(err <= err_persent , "err: " << err << " error %: " << err_persent);

}

BOOST_AUTO_TEST_CASE( test_corrupt_case_2 )
{
    corruptor corr;
    std::vector<uint8_t> b;
    b.push_back(0xFF);

    std::string bits;
    to_string(corr.to_bitset(b),bits);
    BOOST_CHECK_MESSAGE(bits == "11111111", "bits = " << bits);

    b.clear();
    b.push_back(0xF0);
    to_string(corr.to_bitset(b), bits);
    BOOST_CHECK_MESSAGE(bits == "11110000", "bits = " << bits);

    b.clear();
    b.push_back(0xF0);
    b.push_back(0x0F);
    to_string(corr.to_bitset(b), bits);
    BOOST_CHECK_MESSAGE(bits == "0000111111110000", "bits = " << bits);

}

BOOST_AUTO_TEST_CASE( test_corrupt_case_3 )
{
    corruptor corr;

    std::vector<uint8_t> data;
    data.push_back(0xFF);

    boost::dynamic_bitset<> bits = corr.to_bitset(data);

    BOOST_CHECK(bits.size() == 8);
    std::vector<uint8_t> conv = corr.to_bytes(bits);
    BOOST_CHECK_MESSAGE(conv.size() == data.size(), "conv size = " << conv.size());
    BOOST_CHECK(data == conv);

    data.clear();
    data.push_back(0xff);
    data.push_back(0xf0);
    data.push_back(0x0f);

    bits = corr.to_bitset(data);
    BOOST_CHECK_MESSAGE(bits.size() == (data.size() * 8), "bits size = " << bits.size()
            << " data size = " << data.size());
    conv = corr.to_bytes(bits);
    BOOST_CHECK(data == conv);

}

BOOST_AUTO_TEST_SUITE_END()




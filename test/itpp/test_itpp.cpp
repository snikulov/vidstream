#define BOOST_TEST_MODULE test_itpp
#include <boost/test/included/unit_test.hpp>

#include <itpp/itcomm.h>
#include <boost/dynamic_bitset.hpp>
#include <utils/converters.hpp>

#include <algorithm>
#include <cctype>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_suite_itpp)

BOOST_AUTO_TEST_CASE( test_itpp_case_1 )
{
    int num_test = 0xFF;

    itpp::bvec d2b = itpp::dec2bin(num_test);
    int b2d  = itpp::bin2dec(d2b);
    BOOST_CHECK_MESSAGE(num_test == b2d, "num_test = " << num_test << " b2d = " << b2d);

    itpp::BCH bchcodec(15, 7);

    itpp::bvec encoded = bchcodec.encode(d2b);

    BOOST_MESSAGE("d2b.length = " << d2b.length());
    BOOST_MESSAGE("encoded.length = " << encoded.length());

    BOOST_CHECK_MESSAGE(d2b != encoded, "d2b = " << d2b << " encoded = " << encoded);

    itpp::bvec decoded = bchcodec.decode(encoded);
    BOOST_MESSAGE("decoded.length = " << decoded.length());

    BOOST_CHECK_MESSAGE(d2b == decoded, "d2b = " << d2b << " decoded = " << decoded);

    std::string bvstring = itpp::to_str(decoded);
    BOOST_MESSAGE("decoded = " << bvstring);

    std::string bbb(bvstring.begin() + 1, bvstring.end() - 1);
    itpp::bvec k(bbb);
    BOOST_CHECK_MESSAGE(k == decoded, "k = " << k);
}

struct myisdig
{
    bool operator()(char c) const
    {
        return ::isdigit(c);
    }
    typedef char argument_type;
};

BOOST_AUTO_TEST_CASE( test_itpp_case_2 )
{
    int num_test = 0xF1;
    itpp::bvec d2b = itpp::dec2bin(num_test);
    std::string d2bstr = itpp::to_str(d2b);

    // erase all itpp stuff from string
    d2bstr.erase(std::remove_if(d2bstr.begin(), d2bstr.end(), std::not1(myisdig())), d2bstr.end());

    boost::dynamic_bitset<uint8_t> bs(d2bstr);

    std::vector<uint8_t> bv(bs.num_blocks());
    to_block_range(bs, bv.begin());

    BOOST_MESSAGE("bs = " << bs);
    BOOST_MESSAGE("d2bstr = " << d2bstr);

    BOOST_CHECK(bv.size() == 1);
    BOOST_CHECK_MESSAGE(bv.at(0) == num_test, "bv.at(0) = " << std::hex << static_cast<int>(bv.at(0)));

    boost::dynamic_bitset<uint8_t> bs1(bv.begin(), bv.end());
    BOOST_MESSAGE("bs1 = " << bs1);

    std::string bs1str;
    to_string(bs1, bs1str);
    BOOST_MESSAGE("bs1str = " << bs1str);

    itpp::bvec bvec1;
    vidstream::to_itppbvec(bs1, bvec1);

    BOOST_MESSAGE("bvec1 = " << bvec1);

    bs1.clear();
    vidstream::to_dynbitset(bvec1, bs1);
    BOOST_MESSAGE("bs1 = " << bs1);

    uint8_t data_to_sent = 0xFE;
    boost::dynamic_bitset<uint8_t> bstosent(&data_to_sent, &data_to_sent+1);

    BOOST_MESSAGE("bstosent = " << bstosent);

}

BOOST_AUTO_TEST_SUITE_END()




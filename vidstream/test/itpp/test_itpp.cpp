#define BOOST_TEST_MODULE test_perf
#include <boost/test/included/unit_test.hpp>

#include <itpp/itcomm.h>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_suite_itpp)

BOOST_AUTO_TEST_CASE( test_itpp_case_1 )
{
    
    int num_test = 0x0FF0;
    itpp::bvec d2b = itpp::dec2bin(num_test);
    int b2d  = itpp::bin2dec(d2b);
    BOOST_CHECK_MESSAGE(num_test == b2d, "num_test = " << num_test << " b2d = " << b2d);

    itpp::BCH bchcodec(7, 3);

    itpp::bvec encoded = bchcodec.encode(d2b);

    BOOST_MESSAGE("d2b.length = " << d2b.length());
    BOOST_MESSAGE("encoded.length = " << encoded.length());

    BOOST_CHECK_MESSAGE(d2b != encoded, "d2b = " << d2b << " encoded = " << encoded);

    itpp::bvec decoded = bchcodec.decode(encoded);
    BOOST_MESSAGE("decoded.length = " << decoded.length());

    BOOST_CHECK_MESSAGE(d2b == decoded, "d2b = " << d2b << " decoded = " << decoded);

    std::string bvstring = itpp::to_str(decoded);
    BOOST_MESSAGE("decoded = " << bvstring);

    std::string bbb(bvstring.begin()+1, bvstring.end()-1);
    itpp::bvec k(bbb);
    BOOST_CHECK_MESSAGE(k == decoded, "k = " << k);
}

BOOST_AUTO_TEST_SUITE_END()




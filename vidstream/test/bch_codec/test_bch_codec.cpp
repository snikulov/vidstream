#define BOOST_TEST_MODULE test_bch
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <ecc/bch_codec.hpp>
#include <itpp/itcomm.h>

#include <channel/bchwrapper.hpp>


using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_suite_bch)

#if 0
BOOST_AUTO_TEST_CASE( test_bch_case_1 )
{
    uint16_t byte = 0x1FF1;

    // kernel codec
    ecc kc(15, 7);

    // itpp codec
    itpp::BCH ic(15, 7);

    size_t outdatalen = 0;
    char* kdata = kc.encode((char*)&byte, sizeof(byte), outdatalen);

    itpp::bvec bc = itpp::dec2bin(16, byte);
    BOOST_MESSAGE("bc.size() = " << bc.size());

    itpp::bvec ebc = ic.encode(bc);

    BOOST_MESSAGE("outdatalen = " << outdatalen);
    BOOST_MESSAGE("ebc.size() = " << ebc.size());

    BOOST_MESSAGE("ebc = " << ebc);

    std::ostringstream os;
    for(size_t i = 0; i < outdatalen; ++i)
    {
        os << std::hex << (int)kdata[i];
    }
    BOOST_MESSAGE("kdata = " << os.str());

    std::vector<char> vc;
    bool dec_st = false;

    size_t dodl = 0;
    char * dk = kc.decode((unsigned char*)kdata, outdatalen, dodl, vc, dec_st);

    BOOST_MESSAGE("dk = " << (int)(*dk));

    free(kdata);
    free(dk);
}
#endif

BOOST_AUTO_TEST_CASE( test_bch_case_2 )
{
    bchwrapper bch(5, 3);
    std::vector<uint8_t> src;

    src.push_back(0);
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    std::vector<uint8_t> enc_src;
    bch.get()->encode(src, enc_src);

    std::vector<uint8_t> dst;
    bch.get()->decode(enc_src, dst);

    BOOST_CHECK(dst == src);
}


BOOST_AUTO_TEST_SUITE_END()




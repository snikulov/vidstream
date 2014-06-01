#define BOOST_TEST_MODULE test_bch
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <ecc/bch_codec.hpp>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_suite_bch)

BOOST_AUTO_TEST_CASE( test_bch_case_1 )
{
//    BOOST_REQUIRE(framework::master_test_suite().argc > 1);
    std::vector<unsigned char> etal;

    etal.push_back(0xff);
    etal.push_back(0xd1);

    for(unsigned char i = 0; i < 10; ++i)
    {
        etal.push_back(i);
    }

    // mode 1:5 (13, 105) - orig 12, encoded buf len 1024, decoded 853
    // mode 1:1 (5, 3) - orig 12, encoded 24, decoded 12
    // mode 2:1 (6, 6) - orig 12, encoded 32, decoded 12
    // mode 3:1 (5, 4) - orig 12, encoded 32, decoded 12
    bch_codec ecc(5, 4);
    std::vector<unsigned char> ec = ecc.encode(etal);
    BOOST_REQUIRE(ec != etal);

    std::vector<char> good;
    bool is_ok = false;
    std::vector<unsigned char> dc = ecc.decode(ec, good, is_ok);

    BOOST_CHECK(is_ok);
    BOOST_CHECK_MESSAGE(etal == dc, "etal size=" << etal.size() << " dc size=" << dc.size() << " good size=" << good.size());
    for(size_t i=0; i < etal.size(); ++i)
        BOOST_CHECK_MESSAGE(etal.at(i) == dc.at(i),"diff at i="<< i);
    for(size_t i=etal.size(); i < dc.size(); ++i)
        BOOST_CHECK_MESSAGE(dc.at(i) == 0, "dc("<< i << ")="<<dc.at(i));

    BOOST_TEST_MESSAGE("etal size=" << etal.size() << " ec size=" << ec.size() << " dc size=" << dc.size());
}

BOOST_AUTO_TEST_SUITE_END()




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

    corr.add_error(data, 1.);

    BOOST_CHECK(etalon != data);
    double err = corr.check_error(etalon, data);
    int ret = static_cast<int>(err);

    BOOST_CHECK_MESSAGE(ret == 5, "err=" << err << " ret=" << ret);

}

BOOST_AUTO_TEST_SUITE_END()




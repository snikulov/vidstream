#define BOOST_TEST_MODULE test_split
#include <boost/test/included/unit_test.hpp>

#include "split/split.h"

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_suite_split)

BOOST_AUTO_TEST_CASE( test_split_case1 )
{
	BOOST_REQUIRE(framework::master_test_suite().argc > 1);

	std::ifstream input(framework::master_test_suite().argv[1],
		std::ios_base::binary);
	std::ofstream outhead("head", std::ios_base::binary);
	std::ofstream outbody("body", std::ios_base::binary);

	std::size_t rst_count = 0;
	char safe = 1;
	int res = split_file(input, &outhead, outbody, safe, rst_count);

	BOOST_CHECK_EQUAL(res, 1);
	BOOST_TEST_MESSAGE("rst_count: " << rst_count); // 1200 for 640x480
}

BOOST_AUTO_TEST_SUITE_END()




#define BOOST_TEST_MODULE test_split
#include <boost/test/included/unit_test.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <iostream>
#include <sstream>
#include <string>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_suite_config)

BOOST_AUTO_TEST_CASE( test_config_case_1 )
{
    BOOST_REQUIRE(framework::master_test_suite().argc > 1);

    std::ifstream input(framework::master_test_suite().argv[1]);

    boost::property_tree::ptree pt;
    boost::property_tree::read_json(input, pt);

    BOOST_CHECK(9955 == pt.get<int>("cfg.dataport"));
    BOOST_CHECK(5 == pt.get<int>("cfg.bch.m"));
    BOOST_CHECK(103 == pt.get<int>("cfg.bch.t"));

    BOOST_CHECK(640 == pt.get<int>("cfg.img.width"));
    BOOST_CHECK(480 == pt.get<int>("cfg.img.height"));
    BOOST_CHECK(1 == pt.get<int>("cfg.img.rst"));
    BOOST_CHECK(20 == pt.get<int>("cfg.img.lum"));
    BOOST_CHECK(20 == pt.get<int>("cfg.img.chrom"));
    BOOST_CHECK(false == pt.get<bool>("cfg.img.bw"));
}

BOOST_AUTO_TEST_SUITE_END()




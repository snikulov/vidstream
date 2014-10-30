#define BOOST_TEST_MODULE test_perf
#include <boost/test/included/unit_test.hpp>

#include <types.hpp>
#include <jpeg/jpeg_stream_parser.hpp>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_suite_parser)

BOOST_AUTO_TEST_CASE(test_parser_case_1)
{

    std::vector<uint8_t> test_mark(4, 0xff);
    jpeg_stream_parser jstm(test_mark);

    

    BOOST_CHECK_MESSAGE(jstm.parse() == jpeg_stream_parser::need_more_data, "check empty parser");
    BOOST_CHECK(0 == jstm.int_buf_size());

    std::vector<uint8_t> test1(test_mark.begin(), test_mark.end());
    BOOST_CHECK_MESSAGE(jstm.parse(test1) == jpeg_stream_parser::need_more_data, "check empty parser");
    BOOST_CHECK(0 == jstm.int_buf_size());

    std::vector<uint8_t> test2(test_mark.begin(), test_mark.begin()+2);
    BOOST_CHECK_MESSAGE(jstm.parse(test2) == jpeg_stream_parser::need_more_data, "check empty parser");
    BOOST_CHECK_MESSAGE(jstm.int_buf_size() == test2.size(), jstm.int_buf_size() << ", " << test2.size());

    BOOST_CHECK_MESSAGE(jstm.parse(test2) == jpeg_stream_parser::need_more_data, "check empty parser");
    BOOST_CHECK_MESSAGE(jstm.int_buf_size() == 0, jstm.int_buf_size() << ", " << test2.size());

    std::vector<uint8_t> test3(test_mark.begin(), test_mark.end());
    std::string txt("test1");
    test3.insert(test3.end(), txt.begin(), txt.end());
    
    BOOST_CHECK_MESSAGE(jstm.parse(test3) == jpeg_stream_parser::need_more_data, "check empty parser");
    BOOST_CHECK(txt.size() == jstm.int_buf_size());

    BOOST_CHECK_MESSAGE(jstm.parse(test_mark) == jpeg_stream_parser::jpeg_ready, "check empty parser");
    BOOST_CHECK_MESSAGE(0 == jstm.int_buf_size(), "int buf size = " << jstm.int_buf_size());
    BOOST_CHECK(1 == jstm.num_jpegs());

}

BOOST_AUTO_TEST_SUITE_END()




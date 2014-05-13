#define BOOST_TEST_MODULE test_split
#include <boost/test/included/unit_test.hpp>

#include <opencv2/opencv.hpp>

#include <frame.hpp>
#include <types.hpp>

#include <split/split.h>
#include <jpeg_builder.hpp>

using namespace boost::unit_test;
using namespace vidstream;
using namespace cv;

BOOST_AUTO_TEST_SUITE(test_suite_split)

BOOST_AUTO_TEST_CASE( test_split_case_1 )
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

BOOST_AUTO_TEST_CASE( test_get_rst_block_1 )
{
    BOOST_REQUIRE(framework::master_test_suite().argc > 1);

    // TODO: replace to opencv reading from file
    std::ifstream input(framework::master_test_suite().argv[1], std::ios::binary);
    input.unsetf(std::ios::skipws);  // not eat newlines
    std::istream_iterator<unsigned char> start(input), end;
    std::vector<unsigned char> buf(start, end);

    BOOST_REQUIRE(!buf.empty());
    // jpeg header check
    BOOST_REQUIRE( (buf[0] == 0xff) && (buf[1] == 0xd8) );

    std::vector<size_t> outidx;
    BOOST_REQUIRE(outidx.empty());
    BOOST_REQUIRE(get_all_rst_blocks(buf, outidx));
    BOOST_REQUIRE(!outidx.empty());

    BOOST_TEST_MESSAGE("rst_count: " << outidx.size());

    for(size_t i = 0; i < outidx.size(); i++ )
    {
        size_t idx1 = outidx[i];
        size_t idx2 = idx1+1;
        BOOST_CHECK_MESSAGE(buf[idx1] == 0xFF, "buf[" << idx1 << "] != 0xFF");
        BOOST_CHECK_MESSAGE(is_valid_marker(buf[idx2]), "buf[" << idx2 << "] =" << static_cast<int>(buf[i+1]));
    }
}

BOOST_AUTO_TEST_CASE( test_rebuild_image_1 )
{
    BOOST_REQUIRE(framework::master_test_suite().argc > 1);

    camera_frame_t dst(new cv::Mat(480, 640, CV_8UC3, cv::Scalar::all(0)));
    jpeg_builder jbuilder;

    jpeg_data_t out = jbuilder.from_cvmat(dst);
    jbuilder.write(out, 512);

}



BOOST_AUTO_TEST_SUITE_END()




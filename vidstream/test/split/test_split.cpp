#define BOOST_TEST_MODULE test_split
#include <boost/test/included/unit_test.hpp>

#include <opencv2/opencv.hpp>

#include <frame.hpp>
#include <types.hpp>

#include <split/split.h>
#include <jpeg/jpeg_builder.hpp>

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

    jpeg_data_t data = jpeg_builder::read(framework::master_test_suite().argv[1]);

    BOOST_REQUIRE( data );
    BOOST_REQUIRE( !data->empty() );
    // jpeg header check
    BOOST_REQUIRE( (data->at(0) == 0xff) && (data->at(1) == 0xd8) );

    std::vector<size_t> outidx;
    BOOST_REQUIRE(outidx.empty());
    BOOST_REQUIRE(get_all_rst_blocks(*data, outidx));
    BOOST_REQUIRE(!outidx.empty());

    BOOST_TEST_MESSAGE("rst_count: " << outidx.size());

    for(size_t i = 0; i < outidx.size()-1; i++ )
    {
        size_t idx1 = outidx[i];
        size_t idx2 = idx1+1;
        unsigned char val1 = data->at(idx1);
        unsigned char val2 = data->at(idx1+1);
        BOOST_CHECK_MESSAGE(val1 == 0xFF, "data[" << idx1 << "] != 0xFF");
        BOOST_CHECK_MESSAGE(is_valid_marker(val2), "data[" << idx2 << "] =" << static_cast<int>(val2));
        unsigned rstnum = val2 & 0x0F;
        BOOST_CHECK_MESSAGE(i%8 == rstnum, "i="<< i << " rstnum=" <<rstnum);
    }
}

BOOST_AUTO_TEST_CASE( test_rebuild_image_1 )
{
    BOOST_REQUIRE(framework::master_test_suite().argc > 1);
    jpeg_builder jbuilder;

    jpeg_data_t src = jpeg_builder::read(framework::master_test_suite().argv[1]);
    BOOST_REQUIRE( src );
    BOOST_REQUIRE( !src->empty() );

    camera_frame_t frame(new cv::Mat(480, 640, CV_8UC3, cv::Scalar::all(0)));
    jpeg_data_t dst = jbuilder.from_cvmat(frame);

    std::vector<size_t> dst_idxs;
    std::vector<size_t> src_idxs;

    BOOST_REQUIRE(get_all_rst_blocks(*dst, dst_idxs));
    BOOST_REQUIRE(get_all_rst_blocks(*src, src_idxs));

    // erase all destination rst blocks
    dst->erase(dst->begin()+dst_idxs[0], dst->end());

    dst->insert(dst->end(),src->begin()+src_idxs[0], src->end());

    jbuilder.write(dst, 512);

}



BOOST_AUTO_TEST_SUITE_END()




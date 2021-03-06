#define BOOST_TEST_MODULE test_split
#include <boost/test/included/unit_test.hpp>

#include <opencv2/opencv.hpp>

#include <frame.hpp>
#include <types.hpp>

#include <split/split.h>
#include <jpeg/jpeg_builder.hpp>
#include <jpeg/jpeg_rcv_stm.hpp>
#include <jpeg/jpeg_transport.hpp>
#include <jpeg/jpeg_history.hpp>

#include <jpeg/jpeg_stream_parser.hpp>

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

BOOST_AUTO_TEST_CASE( test_get_rst_block_file )
{
    BOOST_REQUIRE(framework::master_test_suite().argc > 1);

    jpeg_data_t data = jpeg_builder::read(framework::master_test_suite().argv[1]);

    BOOST_REQUIRE( data );
    BOOST_REQUIRE( !data->empty() );
    // jpeg header check
    std::vector<uint8_t>& rdata = *data;

    BOOST_CHECK_MESSAGE(rdata[0] == 0xff, "rdata[0] =" << std::hex << (int)rdata[0]);
    BOOST_CHECK_MESSAGE(rdata[1] == 0xd8, "rdata[1] =" << std::hex << (int)rdata[1]);

    std::vector<size_t> outidx;
    BOOST_REQUIRE(get_all_rst_blocks(*data, outidx));
    BOOST_REQUIRE(!outidx.empty());

    BOOST_TEST_MESSAGE("rst_count: " << outidx.size());


    std::vector<uint8_t> rstblocks(rdata.begin()+outidx[0], rdata.begin()+outidx[outidx.size()-1]);

    std::ofstream of("rstblocks", std::ios_base::binary);
    const char * p_buf = reinterpret_cast<const char*>(&rstblocks[0]);
    of.write(p_buf, rstblocks.size()*sizeof(unsigned char));
    of.close();

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

#if 0
BOOST_AUTO_TEST_CASE( test_get_rst_block_2 )
{
    BOOST_REQUIRE(framework::master_test_suite().argc > 1);

    jpeg_data_t data = jpeg_builder::read(framework::master_test_suite().argv[1]);

    BOOST_REQUIRE( data );
    BOOST_REQUIRE( !data->empty() );

    // jpeg header check
    std::vector<size_t> outidx;
    BOOST_REQUIRE(get_all_rst_blocks(*data, outidx));
    BOOST_REQUIRE(!outidx.empty());
    BOOST_TEST_MESSAGE("rst_count: " << outidx.size());

    boost::shared_ptr<jpeg_builder> jb_(new jpeg_builder());
    boost::shared_ptr<jpeg_history> history;
    jpeg_transport jt;
    const std::vector<unsigned char>& s_mark = jt.start_mark();
    const std::vector<unsigned char>& e_mark = jt.end_mark();
    jpeg_rcv_stm stm(jb_, history, s_mark, e_mark);

    BOOST_REQUIRE(STM_WAIT_RST == stm.process(s_mark));

    for(size_t i=0; i<outidx.size()-1; ++i)
    {
        std::vector<uint8_t> buf(data->begin()+outidx[i]+2, data->begin()+outidx[i+1]);
        BOOST_CHECK(STM_WAIT_RST == stm.process(buf));
    }
    BOOST_CHECK(stm.has_data());
    jb_->write(stm.get_jpeg(), 522);

}
#endif

BOOST_AUTO_TEST_CASE(test_get_rst_block_3)
{
    BOOST_REQUIRE(framework::master_test_suite().argc > 1);

    jpeg_data_t data = jpeg_builder::read(framework::master_test_suite().argv[1]);

    BOOST_REQUIRE(data);
    BOOST_REQUIRE(!data->empty());

    // jpeg header check
    std::vector<size_t> outidx;
    BOOST_REQUIRE(get_all_rst_blocks(*data, outidx));
    BOOST_REQUIRE(!outidx.empty());
    BOOST_TEST_MESSAGE("rst_count: " << outidx.size());


    boost::shared_ptr<jpeg_builder> jb_(new jpeg_builder());
    boost::shared_ptr<jpeg_history> history;
    jpeg_transport jt;
    const std::vector<unsigned char>& s_mark = jt.start_mark();

    std::vector<uint8_t> data_to_parse(s_mark.begin(), s_mark.end());
    std::vector<uint8_t>& dref = *data;
    data_to_parse.insert(data_to_parse.end(), &dref[outidx[0]], &dref[outidx[outidx.size()-1]]);
    data_to_parse.insert(data_to_parse.end(), s_mark.begin(), s_mark.end());

    jpeg_stream_parser stream_parser(jt.start_mark());

    BOOST_CHECK(jpeg_stream_parser::jpeg_ready == stream_parser.parse(data_to_parse));


    jb_->write(jb_->build_jpeg_from_rst(stream_parser.get_jpeg()), 533);

}



BOOST_AUTO_TEST_SUITE_END()




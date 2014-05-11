#ifndef JPEG_BUILDER_HPP__
#define JPEG_BUILDER_HPP__

//
// Stateless builder for jpeg memory buffer with specific parameters
//

#include <frame.hpp>

namespace vidstream
{

typedef boost::shared_ptr<std::vector<unsigned char> > jpeg_data_t;
class jpeg_builder
{
public:
    jpeg_builder(int quality=100, int rst_interval=1, int lum=20, int chrom= 20)
    {
        params_.push_back(CV_IMWRITE_JPEG_QUALITY);
        params_.push_back(quality);
        params_.push_back(CV_IMWRITE_JPEG_RST_INTERVAL);
        params_.push_back(rst_interval);
        params_.push_back(CV_IMWRITE_JPEG_LUM_QUALITY);
        params_.push_back(lum);
        params_.push_back(CV_IMWRITE_JPEG_CHROM_QUALITY);
        params_.push_back(chrom);
    }

    ~jpeg_builder()
    {
    }

    jpeg_data_t from_cvmat(const camera_frame_t frame)
    {
        jpeg_data_t ret_buf(new std::vector<unsigned char>);
        if (frame && !frame->empty())
        {
            bool res = cv::imencode(".jpg", *frame, *ret_buf, params_);
        }
        return ret_buf;
    }

private:
    /* data */
    std::vector<int> params_;
};
} /* namespace vidstream */
#endif // JPEG_BUILDER_HPP__

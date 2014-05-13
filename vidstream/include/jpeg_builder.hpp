#ifndef JPEG_BUILDER_HPP__
#define JPEG_BUILDER_HPP__

//
// Stateless builder for jpeg memory buffer with specific parameters
//
#include <iomanip>
#include <fstream>
#include <sstream>

#include <frame.hpp>
#include <types.hpp>

#include <split/split.h>

namespace vidstream
{


// TODO: add interface to access jpeg parameters
// - get/set

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

    jpeg_rst_idxs_t rst_idxs(jpeg_data_t data)
    {
        jpeg_rst_idxs_t ret_val(new std::vector<std::size_t>);
        if (data && !data->empty())
        {
            (void)get_all_rst_blocks(*data, *ret_val);
        }
        return ret_val;
    }

    jpeg_data_t from_cvmat(const camera_frame_t frame)
    {
        jpeg_data_t ret_buf(new std::vector<unsigned char>);
        if (frame && !frame->empty())
        {
            bool res = cv::imencode(".jpg", *frame, *ret_buf, params_);
            // TODO: need to decide what must I do in this case
        }
        return ret_buf;
    }

    std::vector<int> get_params()
    {
        return params_;
    }

    // TODO: should it be in debug only?
    // write to file named "img[num].jpg"
    static void write(const jpeg_data_t data, unsigned long num)
    {
        std::ostringstream fn;
        fn << "img" << std::setfill ('0') << std::setw(8)
           << num << ".jpg";
        std::ofstream of(fn.str().c_str(), std::ios_base::binary);
        const char * p_buf = reinterpret_cast<const char*>(&((*data)[0]));
        of.write(p_buf, data->size()*sizeof(unsigned char));
        of.close();
    }

    static jpeg_data_t read(const std::string& fname)
    {
        jpeg_data_t ret_val;
        std::ifstream in(fname.c_str(), std::ios::binary);
        if (in)
        {
            in.unsetf(std::ios::skipws);
            std::istream_iterator<unsigned char> start(in), end;
            ret_val.reset(new std::vector<unsigned char>(start, end));
        }
        return ret_val;
    }

    jpeg_data_t build_jpeg_from_rst(jpeg_data_t jpeg_rst)
    {
        // Hard coded for now
        camera_frame_t frame(new cv::Mat(480, 640, CV_8UC3, cv::Scalar::all(0)));

        jpeg_data_t dst = from_cvmat(frame);
        std::vector<size_t> dst_idxs;
        (void)get_all_rst_blocks(*dst, dst_idxs);


        dst->erase(dst->begin()+dst_idxs[0], dst->end());
        dst->insert(dst->end(),jpeg_rst->begin(), jpeg_rst->end());

        return dst;
    }

private:
    /* data */
    std::vector<int> params_;
};
} /* namespace vidstream */
#endif // JPEG_BUILDER_HPP__

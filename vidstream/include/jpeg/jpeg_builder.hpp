#ifndef JPEG_BUILDER_HPP__
#define JPEG_BUILDER_HPP__

//
// Stateless builder for jpeg memory buffer with specific parameters
//
#include <iomanip>
#include <fstream>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <frame.hpp>
#include <types.hpp>

#include <split/split.h>
#include <cfg/cfg_notify.hpp>

namespace vidstream
{


// TODO: add interface to access jpeg parameters
// - get/set

class jpeg_builder : public cfg_notify
{
public:
    jpeg_builder(int quality=100, int rst_interval=1, int lum=20, int chrom= 20)
        : quality_(quality), csize_(cv::Size(640, 480))
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
        camera_frame_t frame(new cv::Mat(csize_, CV_8UC3, cv::Scalar::all(0)));

        jpeg_data_t dst = from_cvmat(frame);
        std::vector<size_t> dst_idxs;
        (void)get_all_rst_blocks(*dst, dst_idxs);


        dst->erase(dst->begin()+dst_idxs[0], dst->end());
        dst->insert(dst->end(),jpeg_rst->begin(), jpeg_rst->end());

        return dst;
    }

    void cfg_changed(const boost::property_tree::ptree& cfg)
    {
        int w = cfg.get<int>("cfg.img.width");
        int h = cfg.get<int>("cfg.img.height");
        cv::Size tmps(w, h);

        if (csize_ != tmps)
        {
            csize_ = tmps;
        }

        std::vector<int> tpar;
        tpar.push_back(CV_IMWRITE_JPEG_QUALITY);
        tpar.push_back(quality_);
        tpar.push_back(CV_IMWRITE_JPEG_RST_INTERVAL);
        tpar.push_back(cfg.get<int>("cfg.img.rst"));
        tpar.push_back(CV_IMWRITE_JPEG_LUM_QUALITY);
        tpar.push_back(cfg.get<int>("cfg.img.lum"));
        tpar.push_back(CV_IMWRITE_JPEG_CHROM_QUALITY);
        tpar.push_back(cfg.get<int>("cfg.img.chrom"));

        if(params_ != tpar)
        {
            params_.swap(tpar);
        }
    }

private:
    /* data */
    int quality_;
    std::vector<int> params_;
    cv::Size csize_;
};
} /* namespace vidstream */
#endif // JPEG_BUILDER_HPP__
